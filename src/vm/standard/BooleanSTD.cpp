#include "BooleanSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"

namespace ls {

BooleanSTD::BooleanSTD() : Module("Boolean") {

	operator_("+", {
		{Type::BOOLEAN, Type::STRING, Type::STRING_TMP, (void*) &BooleanSTD::add},
		{Type::BOOLEAN, Type::STRING_TMP, Type::STRING_TMP, (void*) &BooleanSTD::add_tmp}
	});

	static_method("compare", {
		{Type::INTEGER, {Type::POINTER, Type::POINTER}, (void*) &BooleanSTD::compare_ptr_ptr},
		{Type::INTEGER, {Type::BOOLEAN, Type::BOOLEAN}, (void*) &BooleanSTD::compare_val_val, Method::NATIVE}
	});

	method("compare", {
		{Type::POINTER, Type::INTEGER, {Type::POINTER}, (void*) &BooleanSTD::compare_ptr_ptr},
		{Type::BOOLEAN, Type::INTEGER, {Type::BOOLEAN}, (void*) &BooleanSTD::compare_val_val, Method::NATIVE}
	});
}

LSString* BooleanSTD::add(int boolean, LSString* string) {
	return new LSString((boolean ? "true" : "false") + *string);
}
LSString* BooleanSTD::add_tmp(int boolean, LSString* string) {
	(*string).insert(0, (boolean ? "true" : "false"));
	return string;
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

Compiler::value BooleanSTD::compare_val_val(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_cmpl(c.F, args[0].v, args[1].v), Type::INTEGER};
}

}
