#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace parser {

void Parser::addTokens(std::vector<lexer::Token> newTokens) {
  tokens.insert(tokens.end(), std::make_move_iterator(newTokens.begin()),
                std::make_move_iterator(newTokens.end()));
}

const lexer::Token &Parser::current() const { return tokens[pos]; }

bool Parser::check(lexer::TokenType type) const {
  return current().type == type;
}

bool Parser::match(lexer::TokenType type) {
  if (check(type)) {
    ++pos;
    return true;
  }
  return false;
}

const lexer::Token &Parser::consume(lexer::TokenType type) {
  if (!check(type)) {
    throw std::runtime_error("Unexpected token: " + current().lexeme);
  }
  return tokens[pos++];
}

std::unique_ptr<ast::Expr> Parser::parsePrimary() {
  if (check(lexer::TokenType::Number)) {
    int value = std::stoi(current().lexeme);
    ++pos;

    auto node = std::make_unique<ast::Number>();
    node->value = value;

    return node;
  }

  if (check(lexer::TokenType::Identifier)) {
    auto node = std::make_unique<ast::Identifier>();
    node->name = current().lexeme;
    ++pos;

    return node;
  }

  if (match(lexer::TokenType::LeftParen)) {
    auto expr = parseExpression();
    consume(lexer::TokenType::RightParen);
    return expr;
  }

  throw std::runtime_error("Expected expression, got: " + current().lexeme);
}

std::unique_ptr<ast::Expr> Parser::parseUnary() {
  if (match(lexer::TokenType::Minus)) {
    auto node = std::make_unique<ast::UnaryExpr>();

    node->op = lexer::TokenType::Minus;
    node->right = parseUnary();
  }

  return parsePrimary();
}

std::unique_ptr<ast::Expr> Parser::parseFactor() {
  auto expr = parseUnary();

  while (check(lexer::TokenType::Star) || check(lexer::TokenType::Slash)) {
    auto op = current().type;
    ++pos;

    auto node = std::make_unique<ast::BinaryExpr>();

    node->op = op;
    node->left = std::move(expr);
    node->right = parseUnary();

    expr = std::move(node);
  }

  return expr;
}

std::unique_ptr<ast::Expr> Parser::parseTerm() {
  auto expr = parseFactor();

  while (check(lexer::TokenType::Plus) || check(lexer::TokenType::Minus)) {
    auto op = current().type;
    ++pos;

    auto node = std::make_unique<ast::BinaryExpr>();

    node->op = op;
    node->left = std::move(expr);
    node->right = parseFactor();

    expr = std::move(node);
  }

  return expr;
}

std::unique_ptr<ast::Expr> Parser::parseComparison() {
  auto expr = parseTerm();

  while (match(lexer::TokenType::Greater)) {
    auto node = std::make_unique<ast::BinaryExpr>();

    node->op = lexer::TokenType::Greater;
    node->left = std::move(expr);
    node->right = parseTerm();

    expr = std::move(node);
  }

  return expr;
}

std::unique_ptr<ast::Expr> Parser::parseEquality() {
  auto expr = parseComparison();

  while (match(lexer::TokenType::Equal)) {
    auto node = std::make_unique<ast::BinaryExpr>();

    node->op = lexer::TokenType::Equal;
    node->left = std::move(expr);
    node->right = parseComparison();

    expr = std::move(node);
  }

  return expr;
}

std::unique_ptr<ast::Expr> Parser::parseExpression() { return parseEquality(); }

std::unique_ptr<ast::Stmt> Parser::parseStatement() {
  std::string name = consume(lexer::TokenType::Identifier).lexeme;

  consume(lexer::TokenType::Assign);

  auto stmt = std::make_unique<ast::Assignment>();
  stmt->name = std::move(name);
  stmt->value = parseExpression();

  consume(lexer::TokenType::Semicolon);
  return stmt;
}

std::unique_ptr<ast::Function> Parser::parseFunction() {
  auto function = std::make_unique<ast::Function>();

  function->name = consume(lexer::TokenType::Identifier).lexeme;

  consume(lexer::TokenType::LeftParen);
  if (!check(lexer::TokenType::RightParen)) {
    do {
      function->parameters.push_back(
          consume(lexer::TokenType::Identifier).lexeme);
    } while (match(lexer::TokenType::Comma));
  }

  consume(lexer::TokenType::RightParen);
  consume(lexer::TokenType::LeftBrace);

  if (match(lexer::TokenType::Var)) {
    do {
      function->variables.push_back(
          consume(lexer::TokenType::Identifier).lexeme);
    } while (match(lexer::TokenType::Comma));
  }

  consume(lexer::TokenType::Semicolon);

  while (!check(lexer::TokenType::Return)) {
    function->statements.push_back(parseStatement());
  }

  consume(lexer::TokenType::Return);

  function->returnValue = parseExpression();

  consume(lexer::TokenType::Semicolon);
  consume(lexer::TokenType::RightBrace);

  return function;
}

std::unique_ptr<ast::Program> Parser::parse() {
  auto program = std::make_unique<ast::Program>();

  while (!check(lexer::TokenType::End)) {
    program->functions.push_back(parseFunction());
  }

  return program;
}

} // namespace parser
