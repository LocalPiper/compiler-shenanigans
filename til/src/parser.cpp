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

  if (match(lexer::TokenType::Input)) {
    auto node = std::make_unique<ast::Input>();
    return node;
  }

  if (match(lexer::TokenType::Null)) {
    return std::make_unique<ast::Null>();
  }

  if (match(lexer::TokenType::LeftParen)) {
    auto expr = parseExpression();

    consume(lexer::TokenType::RightParen);

    return expr;
  }

  if (match(lexer::TokenType::LeftBrace)) {
    auto node = std::make_unique<ast::Record>();

    node->fields.push_back(
        {consume(lexer::TokenType::Identifier).lexeme, nullptr});

    consume(lexer::TokenType::Colon);

    node->fields.back().second = parseExpression();

    while (match(lexer::TokenType::Comma)) {
      std::string name = consume(lexer::TokenType::Identifier).lexeme;

      consume(lexer::TokenType::Colon);

      auto value = parseExpression();

      node->fields.push_back({std::move(name), std::move(value)});
    }

    consume(lexer::TokenType::RightBrace);

    return node;
  }

  throw std::runtime_error("Expected expression, got: " + current().lexeme);
}

std::unique_ptr<ast::Expr> Parser::parsePostfix() {
  auto expr = parsePrimary();

  while (true) {
    if (match(lexer::TokenType::Dot)) {
      auto node = std::make_unique<ast::FieldAccess>();

      node->object = std::move(expr);
      node->field = consume(lexer::TokenType::Identifier).lexeme;

      expr = std::move(node);
      continue;
    }

    if (match(lexer::TokenType::LeftParen)) {
      auto node = std::make_unique<ast::Call>();

      node->function = std::move(expr);

      if (!check(lexer::TokenType::RightParen)) {
        do {
          node->arguments.push_back(parseExpression());
        } while (match(lexer::TokenType::Comma));
      }

      consume(lexer::TokenType::RightParen);

      expr = std::move(node);
      continue;
    }

    break;
  }

  return expr;
}

std::unique_ptr<ast::Expr> Parser::parseUnary() {
  if (match(lexer::TokenType::Minus)) {
    auto node = std::make_unique<ast::UnaryExpr>();

    node->op = lexer::TokenType::Minus;
    node->right = parseUnary();

    return node;
  }

  if (match(lexer::TokenType::Alloc)) {
    auto node = std::make_unique<ast::Alloc>();

    node->value = parseUnary();

    return node;
  }

  if (match(lexer::TokenType::Ampersand)) {
    auto node = std::make_unique<ast::AddressOf>();

    node->value = parseUnary();

    return node;
  }

  if (match(lexer::TokenType::Star)) {
    auto node = std::make_unique<ast::Dereference>();

    node->value = parseUnary();

    return node;
  }

  return parsePostfix();
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

std::unique_ptr<ast::Stmt> Parser::parseIdentifierStatement() {
  std::string name = consume(lexer::TokenType::Identifier).lexeme;

  if (match(lexer::TokenType::Dot)) {
    auto stmt = std::make_unique<ast::FieldAssignment>();

    stmt->object = std::make_unique<ast::Identifier>();

    static_cast<ast::Identifier *>(stmt->object.get())->name = std::move(name);

    stmt->field = consume(lexer::TokenType::Identifier).lexeme;

    consume(lexer::TokenType::Assign);

    stmt->value = parseExpression();

    consume(lexer::TokenType::Semicolon);

    return stmt;
  }

  consume(lexer::TokenType::Assign);

  auto stmt = std::make_unique<ast::Assignment>();

  stmt->name = std::move(name);
  stmt->value = parseExpression();

  consume(lexer::TokenType::Semicolon);

  return stmt;
}

std::unique_ptr<ast::Stmt> Parser::parsePointerAssignment() {
  auto pointer = parseExpression();

  consume(lexer::TokenType::Assign);

  auto stmt = std::make_unique<ast::PointerAssignment>();

  stmt->pointer = std::move(pointer);
  stmt->value = parseExpression();

  consume(lexer::TokenType::Semicolon);

  return stmt;
}

std::unique_ptr<ast::Stmt> Parser::parseParenthesizedFieldAssignment() {
  consume(lexer::TokenType::LeftParen);

  auto object = parseExpression();

  consume(lexer::TokenType::RightParen);
  consume(lexer::TokenType::Dot);

  auto stmt = std::make_unique<ast::FieldAssignment>();

  stmt->object = std::move(object);
  stmt->field = consume(lexer::TokenType::Identifier).lexeme;

  consume(lexer::TokenType::Assign);

  stmt->value = parseExpression();

  consume(lexer::TokenType::Semicolon);

  return stmt;
}

std::vector<std::unique_ptr<ast::Stmt>> Parser::parseBlock() {
  consume(lexer::TokenType::LeftBrace);

  std::vector<std::unique_ptr<ast::Stmt>> statements;

  while (!check(lexer::TokenType::RightBrace)) {
    statements.push_back(parseStatement());
  }

  consume(lexer::TokenType::RightBrace);

  return statements;
}

std::unique_ptr<ast::Stmt> Parser::parseStatement() {
  if (check(lexer::TokenType::Identifier)) {
    return parseIdentifierStatement();
  }

  if (match(lexer::TokenType::Output)) {
    auto stmt = std::make_unique<ast::Output>();

    stmt->value = parseExpression();

    consume(lexer::TokenType::Semicolon);

    return stmt;
  }

  if (match(lexer::TokenType::If)) {
    auto stmt = std::make_unique<ast::If>();

    consume(lexer::TokenType::LeftParen);

    stmt->condition = parseExpression();

    consume(lexer::TokenType::RightParen);

    stmt->thenStatements = parseBlock();

    if (match(lexer::TokenType::Else)) {
      stmt->elseStatements = parseBlock();
    }

    return stmt;
  }

  if (match(lexer::TokenType::While)) {
    auto stmt = std::make_unique<ast::While>();

    consume(lexer::TokenType::LeftParen);

    stmt->condition = parseExpression();

    consume(lexer::TokenType::RightParen);

    stmt->statements = parseBlock();

    return stmt;
  }

  if (check(lexer::TokenType::Star)) {
    return parsePointerAssignment();
  }

  if (check(lexer::TokenType::LeftParen)) {
    return parseParenthesizedFieldAssignment();
  }

  throw std::runtime_error("Expected statement, got: " + current().lexeme);
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