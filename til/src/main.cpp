#include "ast_printer.hpp"
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

  parser.addTokens({{lexer::TokenType::End, ""}});
  auto ast = parser.parse();
  ast_printer::Printer printer;
  printer.print(*ast);
  return 0;
}
