#include "BooleanSTD.hpp"
#include "../value/LSBoolean.hpp"

namespace ls {

BooleanSTD::BooleanSTD() : Module("Boolean") {

	static_method("compare", {
		{Type::INTEGER, {Type::POINTER, Type::POINTER}, (void*) &BooleanSTD::compare_ptr_ptr},
		{Type::INTEGER, {Type::BOOLEAN, Type::BOOLEAN}, (void*) &BooleanSTD::compare_val_val, Method::NATIVE}
	});
}

int BooleanSTD::compare_ptr_ptr(LSBoolean* a, LSBoolean* b) {
	int res = 0;
	if (a->value) {
		if (not b->value) res = 1;
	} else {
		if (b->value) res = -1;
	}
	LSValue::delete_temporary(a);
	LSValue::delete_temporary(b);
	return res;
}

jit_value_t BooleanSTD::compare_val_val(Compiler& c, std::vector<jit_value_t> args) {
	return jit_insn_cmpl(c.F, args[0], args[1]);
}

}
