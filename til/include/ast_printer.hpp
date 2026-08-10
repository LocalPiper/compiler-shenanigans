#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <iostream>

namespace ast_printer {

using namespace ast;
class Printer : public Visitor {
public:
  void visit(Number &node) override {
    std::cout << "Number(" << node.value << ")";
  }

  void visit(Identifier &node) override {
    std::cout << "Identifier(" << node.name << ")";
  }

  void visit(UnaryExpr &node) override {
    std::cout << "UnaryExpr(" << lexer::tokenName(node.op) << ")\n";
    ++indent;
    print(node.right);
    --indent;
  }

  void visit(BinaryExpr &node) override {
    std::cout << "BinaryExpr(" << lexer::tokenName(node.op) << ")\n";
    ++indent;
    print(node.left);
    std::cout << '\n';
    print(node.right);
    --indent;
  }

  void visit(Assignment &node) override {
    std::cout << "Assignment(" << node.name << ")\n";
    ++indent;
    print(node.value);
    --indent;
  }

  void visit(Return &node) override {
    std::cout << "Return()\n";
    ++indent;
    print(node.value);
    --indent;
  }

  void visit(Function &node) override {
    std::cout << "Function(" << node.name << ")";
    ++indent;

    if (!node.parameters.empty()) {
      std::cout << '\n';
      printIndent();
      std::cout << "Parameters(";

      ++indent;
      for (const auto &param : node.parameters) {
        std::cout << '\n';
        printIndent();
        std::cout << "Identifier(" << param << ")";
      }
      std::cout << '\n';
      --indent;
      printIndent();
      std::cout << ')';
    }

    if (!node.variables.empty()) {
      std::cout << '\n';
      printIndent();
      std::cout << "Variables(";

      ++indent;
      for (const auto &var : node.variables) {
        std::cout << '\n';
        printIndent();
        std::cout << "Identifier(" << var << ")";
      }
      std::cout << '\n';
      --indent;
      printIndent();
      std::cout << ')';
    }

    for (const auto &stmt : node.statements) {
      std::cout << '\n';
      print(stmt);
    }

    std::cout << '\n';
    printIndent();
    std::cout << "Return";
    ++indent;
    std::cout << '\n';
    print(node.returnValue);
    indent -= 2;
  }

  void visit(Program &node) override {
    std::cout << "Program()";
    ++indent;
    for (const auto &f : node.functions) {
      std::cout << '\n';
      print(f);
    }
    --indent;
  }

  void print(Node &node) { node.accept(*this); }

private:
  int indent = 0;

  template <typename T> void print(const std::unique_ptr<T> &node) {
    printIndent();
    print(*node);
  }

  void printIndent() { std::cout << std::string(2 * indent, ' '); }
};
} // namespace ast_printer
