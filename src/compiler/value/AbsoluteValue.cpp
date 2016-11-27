#include "../../compiler/value/AbsoluteValue.hpp"

#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
	type = Type::INTEGER;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int, bool debug) const {
	os << "|";
	expression->print(os, debug);
	os << "|";
}

unsigned AbsoluteValue::line() const {
	return 0;
}

void AbsoluteValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	expression->analyse(analyser, Type::POINTER);
	constant = expression->constant;

	type.nature = req_type.nature;
}

LSValue* abso(LSValue* v) {
	return v->abso();
}

Compiler::value AbsoluteValue::compile(Compiler& c) const {

	auto ex = expression->compile(c);

	jit_type_t args_types[2] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);

	jit_value_t absolute_value = jit_insn_call_native(c.F, "abso", (void*) abso, sig, &ex.v, 1, JIT_CALL_NOTHROW);

	VM::delete_temporary(c.F, ex.v);

	return {absolute_value, Type::INTEGER};
}

}
