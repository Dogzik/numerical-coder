#include <fstream>
#include <iostream>
#include <string_view>

#include "coder/decoding.h"
#include "coder/encoding.h"

using namespace std::literals;

void print_usage() {
  std::cout << "Expected 3 arguments: <mode> <input-file> <output-file>\n";
  std::cout << "mode = -c|--compress|-d|-decompress\n";
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    print_usage();
    return 0;
  }
  std::ifstream input(argv[2], std::ios_base::in | std::ios_base::binary);
  std::ofstream output(argv[3], std::ios_base::out | std::ios_base::binary);
  if (!input.is_open()) {
    std::cout << "Couldn't open input file: " << argv[2] << "\n";
    return 0;
  }
  if (!output.is_open()) {
    std::cout << "Couldn't open output file: " << argv[3] << "\n";
    return 0;
  }
  auto mode = argv[1];
  if ((mode == "-c"sv) || (mode == "--compress"sv)) {
    encode(input, output);
  } else if ((mode == "-d"sv) || (mode == "--decompress"sv)) {
    decode(input, output);
  } else {
    std::cout << "Wrong mode: " << mode << "\n";
    print_usage();
  }
  return 0;
}
