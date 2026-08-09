#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

int main() {
  std::string line;
  lexer::Lexer lexer;
  parser::Parser parser;
  while (std::getline(std::cin, line)) {
    lexer.setSource(std::move(line));
    parser.addTokens(lexer.tokenize());
  }

  return 0;
}
