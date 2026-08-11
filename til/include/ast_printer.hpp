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

  void visit(Alloc &node) override {
    std::cout << "Alloc()\n";
    ++indent;
    print(node.value);
    --indent;
  }

  void visit(AddressOf &node) override {
    std::cout << "AddressOf()\n";
    ++indent;
    print(node.value);
    --indent;
  }

  void visit(Dereference &node) override {
    std::cout << "Dereference()\n";
    ++indent;
    print(node.value);
    --indent;
  }

  void visit(Call &node) override {
    std::cout << "Call()";

    ++indent;

    std::cout << '\n';
    printIndent();
    std::cout << "Function\n";

    ++indent;
    print(node.function);
    --indent;

    if (!node.arguments.empty()) {
      std::cout << '\n';
      printIndent();
      std::cout << "Arguments";

      ++indent;

      for (const auto &argument : node.arguments) {
        std::cout << '\n';
        print(argument);
      }

      --indent;
    }

    --indent;
  }

  void visit(Null &) override { std::cout << "Null()"; }

  void visit(Input &) override { std::cout << "Input()"; }

  void visit(Record &node) override {
    std::cout << "Record()";

    ++indent;
    for (const auto &[name, value] : node.fields) {
      std::cout << '\n';
      printIndent();
      std::cout << "Field(" << name << ")\n";

      ++indent;
      print(value);
      --indent;
    }
    --indent;
  }

  void visit(FieldAccess &node) override {
    std::cout << "FieldAccess(" << node.field << ")\n";

    ++indent;
    print(node.object);
    --indent;
  }

  void visit(PointerAssignment &node) override {
    std::cout << "PointerAssignment()\n";

    ++indent;
    print(node.pointer);
    std::cout << '\n';
    print(node.value);
    --indent;
  }

  void visit(FieldAssignment &node) override {
    std::cout << "FieldAssignment(" << node.field << ")\n";

    ++indent;
    print(node.object);
    std::cout << '\n';
    print(node.value);
    --indent;
  }

  void visit(Output &node) override {
    std::cout << "Output()\n";

    ++indent;
    print(node.value);
    --indent;
  }

  void visit(If &node) override {
    std::cout << "If()\n";

    ++indent;

    printIndent();
    std::cout << "Condition\n";
    ++indent;
    print(node.condition);
    --indent;

    if (!node.thenStatements.empty()) {
      std::cout << '\n';
      printIndent();
      std::cout << "Then";
      ++indent;

      for (const auto &stmt : node.thenStatements) {
        std::cout << '\n';
        print(stmt);
      }

      --indent;
    }

    if (!node.elseStatements.empty()) {
      std::cout << '\n';
      printIndent();
      std::cout << "Else";
      ++indent;

      for (const auto &stmt : node.elseStatements) {
        std::cout << '\n';
        print(stmt);
      }

      --indent;
    }

    --indent;
  }

  void visit(While &node) override {
    std::cout << "While()\n";

    ++indent;

    printIndent();
    std::cout << "Condition\n";

    ++indent;
    print(node.condition);
    --indent;

    for (const auto &stmt : node.statements) {
      std::cout << '\n';
      print(stmt);
    }

    --indent;
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
