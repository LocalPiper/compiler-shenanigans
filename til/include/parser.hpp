#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <cstdlib>
#include <memory>
#include <vector>

namespace parser {
class Parser {
public:
  void addTokens(std::vector<lexer::Token> newTokens);
  std::unique_ptr<ast::Program> parse();

private:
  std::vector<lexer::Token> tokens;
  std::size_t pos = 0;

  std::unique_ptr<ast::Function> parseFunction();
  std::unique_ptr<ast::Stmt> parseStatement();
  std::unique_ptr<ast::Expr> parseExpression();

  std::unique_ptr<ast::Expr> parseEqualty();
  std::unique_ptr<ast::Expr> parseComparison();
  std::unique_ptr<ast::Expr> parseTerm();
  std::unique_ptr<ast::Expr> parseFactor();
  std::unique_ptr<ast::Expr> parsePrimary();
};
} // namespace parser
