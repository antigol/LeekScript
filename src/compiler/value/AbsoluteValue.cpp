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

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type&) {

	type = Type::VAR;

	expression->analyse(analyser, Type::VAR);
	if (expression->type != Type::VAR) {
		analyser->add_error({ SemanticException::TYPE_MISMATCH });
	}

	constant = expression->constant;
}

LSVar* AV_abso(LSVar* v) {
	return v->ls_abso();
}

jit_value_t AbsoluteValue::compile(Compiler& c) const {

	jit_value_t ex = expression->compile(c);

	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);

	return jit_insn_call_native(c.F, "abso", (void*) AV_abso, sig, &ex, 1, JIT_CALL_NOTHROW);
}

}
