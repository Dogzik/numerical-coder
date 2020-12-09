#include "coder/encoding.h"

#include <algorithm>
#include <array>
#include <vector>

#include "model/byte-model.h"
#include "model/constants.h"
#include "model/uniform-model.h"

#include "coder/arithmetic-encoder.h"

namespace {

math_t get_stream_size(std::istream &in) {
  in.seekg(0, std::ios::end);
  math_t file_size = in.tellg();
  in.seekg(0, std::ios::beg);
  return file_size;
}

std::vector<std::pair<math_t, math_t>> count_frequencies(std::istream &in) {
  std::array<math_t, 256> frequencies{};
  for (auto raw_byte = in.get(); raw_byte != std::char_traits<char>::eof(); raw_byte = in.get()) {
    unsigned char char_byte = raw_byte;
    ++frequencies[char_byte];
  }
  std::vector<std::pair<math_t, math_t>> res;
  res.reserve(256);
  for (math_t i = 0; i < 256; ++i) {
    if (frequencies[i] != 0) {
      res.emplace_back(frequencies[i], i);
    }
  }
  std::sort(res.begin(), res.end(), std::greater<>());
  in.clear();
  in.seekg(0, std::ios_base::beg);
  return res;
}

void encode_frequencies(arithmetic_encoder &encoder, math_t file_size,
                        const std::vector<std::pair<math_t, math_t>> &frequencies) {
  uniform_model cur_model(file_size);
  for (auto[frequency, symbol] : frequencies) {
    encoder.encode(frequency - 1, cur_model);
    cur_model.set_n(frequency);
    file_size -= frequency;
  }
}

void encode_letters(arithmetic_encoder &encoder, const std::vector<std::pair<math_t, math_t>> &frequencies) {
  byte_model cur_model{};
  for (auto[frequency, symbol] : frequencies) {
    encoder.encode(symbol, cur_model);
  }
}

void encode_text(arithmetic_encoder &encoder, byte_model &text_model, std::istream &text) {
  for (auto raw_byte = text.get(); raw_byte != std::char_traits<char>::eof(); raw_byte = text.get()) {
    unsigned char char_byte = raw_byte;
    encoder.encode(char_byte, text_model);
  }
}

}

void encode(std::istream &input, std::ostream &output) {
  math_t stream_size = get_stream_size(input);
  if (stream_size > MAX_FREQUENCY) {
    throw std::runtime_error("Stream too big to encode. Its size exceeds MAX_FREQUENCY");
  }
  auto frequencies = count_frequencies(input);
  output.write(reinterpret_cast<const char *>(&stream_size), sizeof(stream_size));
  arithmetic_encoder encoder(output);
  encode_frequencies(encoder, stream_size, frequencies);
  encode_letters(encoder, frequencies);
  byte_model text_model(frequencies);
  encode_text(encoder, text_model, input);
}

