#include "../../compiler/value/AbsoluteValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int indent, bool debug) const {
	os << "|";
	expression->print(os, indent, debug);
	os << "|";
}

unsigned AbsoluteValue::line() const {
	return 0;
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type&)
{
	type = Type::VAR;
	expression->analyse(analyser, Type::VAR);
	constant = expression->constant;
}

jit_value_t AbsoluteValue::compile(Compiler& c) const
{
	jit_value_t ex = expression->compile(c);
	return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_abso, { ex });
}

}
