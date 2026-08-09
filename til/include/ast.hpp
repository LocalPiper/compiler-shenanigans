#pragma once
#include "lexer.hpp"
#include <memory>

namespace ast {

struct Visitor;

struct Node {
  virtual ~Node() = default;
  virtual void accept(Visitor &visitor) = 0;
};

struct Expr : Node {};

struct Number : Expr {
  int value;

  void accept(Visitor &v) override;
};

struct Identifier : Expr {
  std::string name;

  void accept(Visitor &v) override;
};

struct UnaryExpr : Expr {
  lexer::TokenType op;
  std::unique_ptr<Expr> right;

  void accept(Visitor &v) override;
};

struct BinaryExpr : Expr {
  lexer::TokenType op;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;

  void accept(Visitor &v) override;
};

struct Stmt : Node {};

struct Assignment : Stmt {
  std::string name;
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct Return : Stmt {
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct Function : Node {
  std::string name;
  std::vector<std::string> parameters;
  std::vector<std::string> variables;
  std::vector<std::unique_ptr<Stmt>> statements;
  std::unique_ptr<Expr> returnValue;

  void accept(Visitor &v) override;
};

struct Program : Node {
  std::vector<std::unique_ptr<Function>> functions;

  void accept(Visitor &v) override;
};

struct Visitor {
  virtual ~Visitor() = default;

  virtual void visit(Number &) = 0;
  virtual void visit(Identifier &) = 0;
  virtual void visit(UnaryExpr &) = 0;
  virtual void visit(BinaryExpr &) = 0;

  virtual void visit(Assignment &) = 0;
  virtual void visit(Return &) = 0;

  virtual void visit(Function &) = 0;
  virtual void visit(Program &) = 0;
};

} // namespace ast
