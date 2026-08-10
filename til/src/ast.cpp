#include "ast.hpp"

namespace ast {
void Number::accept(Visitor &visitor) { visitor.visit(*this); }

void Identifier::accept(Visitor &visitor) { visitor.visit(*this); }

void UnaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }

void BinaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }

void Assignment::accept(Visitor &visitor) { visitor.visit(*this); }

void Return::accept(Visitor &visitor) { visitor.visit(*this); }

void Function::accept(Visitor &visitor) { visitor.visit(*this); }

void Program::accept(Visitor &visitor) { visitor.visit(*this); }
} // namespace ast
