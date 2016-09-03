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

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	type = Type::VAR;
	expression->analyse(analyser, Type::VAR);
	constant = expression->constant;

	if (!type.match_with_generic(req_type, &type)) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

jit_value_t AbsoluteValue::compile(Compiler& c) const
{
	jit_value_t ex = expression->compile(c);
	return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_abso, { ex });
}

}
