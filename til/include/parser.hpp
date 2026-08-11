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

  const lexer::Token &current() const;
  const lexer::Token &consume(lexer::TokenType type);
  bool match(lexer::TokenType type);
  bool check(lexer::TokenType type) const;

  std::unique_ptr<ast::Function> parseFunction();

  std::unique_ptr<ast::Stmt> parseStatement();
  std::vector<std::unique_ptr<ast::Stmt>> parseBlock();

  std::unique_ptr<ast::Expr> parseExpression();
  std::unique_ptr<ast::Expr> parseEquality();
  std::unique_ptr<ast::Expr> parseComparison();
  std::unique_ptr<ast::Expr> parseTerm();
  std::unique_ptr<ast::Expr> parseFactor();
  std::unique_ptr<ast::Expr> parseUnary();
  std::unique_ptr<ast::Expr> parsePostfix();
  std::unique_ptr<ast::Expr> parsePrimary();

  std::unique_ptr<ast::Stmt> parseIdentifierStatement();
  std::unique_ptr<ast::Stmt> parsePointerAssignment();
  std::unique_ptr<ast::Stmt> parseParenthesizedFieldAssignment();
};

} // namespace parser