#include "ast.hpp"

namespace ast {
void Number::accept(Visitor &visitor) { visitor.visit(*this); }

void Identifier::accept(Visitor &visitor) { visitor.visit(*this); }

void UnaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }

void BinaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }

void Alloc::accept(Visitor &visitor) { visitor.visit(*this); }

void AddressOf::accept(Visitor &visitor) { visitor.visit(*this); }

void Dereference::accept(Visitor &visitor) { visitor.visit(*this); }

void Call::accept(Visitor &visitor) { visitor.visit(*this); }

void Null::accept(Visitor &visitor) { visitor.visit(*this); }

void Input::accept(Visitor &visitor) { visitor.visit(*this); }

void Record::accept(Visitor &visitor) { visitor.visit(*this); }

void FieldAccess::accept(Visitor &visitor) { visitor.visit(*this); }

void Assignment::accept(Visitor &visitor) { visitor.visit(*this); }

void PointerAssignment::accept(Visitor &visitor) { visitor.visit(*this); }

void FieldAssignment::accept(Visitor &visitor) { visitor.visit(*this); }

void Output::accept(Visitor &visitor) { visitor.visit(*this); }

void If::accept(Visitor &visitor) { visitor.visit(*this); }

void While::accept(Visitor &visitor) { visitor.visit(*this); }

void Return::accept(Visitor &visitor) { visitor.visit(*this); }

void Function::accept(Visitor &visitor) { visitor.visit(*this); }

void Program::accept(Visitor &visitor) { visitor.visit(*this); }
} // namespace ast
