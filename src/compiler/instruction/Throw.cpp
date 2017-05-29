#include "../../compiler/instruction/Throw.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Throw::Throw(std::shared_ptr<Token> token, Value* v) : token(token) {
	expression = v;
}

Throw::~Throw() {
	delete expression;
}

void Throw::print(ostream& os, int indent, bool debug) const {
	os << "throw";
	if (expression != nullptr) {
		os << " ";
		expression->print(os, indent, debug);
	}
}

Location Throw::location() const {
	return expression->location();
}

void Throw::analyse(SemanticAnalyser* analyser, const Type&) {
	if (expression != nullptr) {
		expression->analyse(analyser, Type::UNKNOWN);
	}
}

Compiler::value Throw::compile(Compiler& c) const {

	auto exception = c.new_integer((int) vm::Exception::EXCEPTION);
	if (expression != nullptr) {
		exception = expression->compile(c);
	}

	jit_insn_mark_offset(c.F, token->location.start.line);

	auto ex = c.insn_call(Type::POINTER, {exception}, &VM::get_exception_object<0>);
	c.insn_throw(ex);

	return {nullptr, Type::UNKNOWN};
}

Instruction* Throw::clone() const {
	auto t = new Throw(token, expression->clone());
	return t;
}

}
