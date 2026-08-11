#pragma once
#include "lexer.hpp"
#include <memory>
#include <string>
#include <vector>

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

struct Alloc : Expr {
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct AddressOf : Expr {
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct Dereference : Expr {
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct Call : Expr {
  std::unique_ptr<Expr> function;
  std::vector<std::unique_ptr<Expr>> arguments;

  void accept(Visitor &v) override;
};

struct Null : Expr {
  void accept(Visitor &v) override;
};

struct Input : Expr {
  void accept(Visitor &v) override;
};

struct Record : Expr {
  std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;

  void accept(Visitor &v) override;
};

struct FieldAccess : Expr {
  std::unique_ptr<Expr> object;
  std::string field;

  void accept(Visitor &v) override;
};

struct Stmt : Node {};

struct Assignment : Stmt {
  std::string name;
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct PointerAssignment : Stmt {
  std::unique_ptr<Expr> pointer;
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct FieldAssignment : Stmt {
  std::unique_ptr<Expr> object;
  std::string field;
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct Output : Stmt {
  std::unique_ptr<Expr> value;

  void accept(Visitor &v) override;
};

struct If : Stmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> thenStatements;
  std::vector<std::unique_ptr<Stmt>> elseStatements;

  void accept(Visitor &v) override;
};

struct While : Stmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> statements;

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

  virtual void visit(Alloc &) = 0;
  virtual void visit(AddressOf &) = 0;
  virtual void visit(Dereference &) = 0;
  virtual void visit(Call &) = 0;
  virtual void visit(Null &) = 0;
  virtual void visit(Input &) = 0;

  virtual void visit(Record &) = 0;
  virtual void visit(FieldAccess &) = 0;

  virtual void visit(Assignment &) = 0;
  virtual void visit(PointerAssignment &) = 0;
  virtual void visit(FieldAssignment &) = 0;
  virtual void visit(Output &) = 0;
  virtual void visit(If &) = 0;
  virtual void visit(While &) = 0;
  virtual void visit(Return &) = 0;

  virtual void visit(Function &) = 0;
  virtual void visit(Program &) = 0;
};

} // namespace ast