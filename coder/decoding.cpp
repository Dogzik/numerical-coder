#include "coder/decoding.h"

#include <vector>

#include "model/byte-model.h"
#include "model/constants.h"
#include "model/uniform-model.h"

#include "coder/arithmetic-decoder.h"

namespace {

std::vector<math_t> decode_frequencies(arithmetic_decoder &decoder, math_t file_size) {
  std::vector<math_t> res;
  uniform_model cur_model(file_size);
  while (file_size > 0) {
    math_t new_frequency = decoder.decode(cur_model, false) + 1;
    res.push_back(new_frequency);
    cur_model.set_n(new_frequency);
    file_size -= new_frequency;
  }
  return res;
}

std::vector<char> decode_letters(arithmetic_decoder &decoder, size_t cnt) {
  std::vector<char> res(cnt);
  byte_model cur_model{};
  for (size_t i = 0; i < cnt; ++i) {
    math_t raw_symbol = decoder.decode(cur_model, false);
    auto symbol = static_cast<char>(raw_symbol);
    res[i] = symbol;
  }
  return res;
}

void decode_text(arithmetic_decoder &decoder, byte_model &text_model, math_t text_size, std::ostream &output) {
  for (math_t i = 0; i < text_size; ++i) {
    math_t raw_symbol = decoder.decode(text_model, i == (text_size - 1));
    auto symbol = static_cast<char>(raw_symbol);
    output.put(symbol);
  }
}

}

void decode(std::istream &input, std::ostream &output) {
  math_t stream_size{};
  input.read(reinterpret_cast<char *>(&stream_size), sizeof(stream_size));
  arithmetic_decoder decoder(input);
  auto frequencies = decode_frequencies(decoder, stream_size);
  auto letters = decode_letters(decoder, frequencies.size());
  byte_model text_model(letters, frequencies);
  decode_text(decoder, text_model, stream_size, output);
}
