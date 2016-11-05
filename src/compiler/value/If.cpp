#include "../../compiler/value/If.hpp"

#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

If::If() {
	elze = nullptr;
	condition = nullptr;
	then = nullptr;
	type = Type::UNKNOWN;
}

If::~If() {
	delete condition;
	delete then;
	if (elze != nullptr) {
		delete elze;
	}
}

void If::print(ostream& os, int indent, bool debug) const {
	os << "if ";
	condition->print(os, indent + 1, debug);
	os << " ";
	then->print(os, indent, debug);
	if (elze != nullptr) {
		os << " else ";
		elze->print(os, indent, debug);
	}
	if (debug) {
		os << " " << type;
	}
}

unsigned If::line() const {
	return 0;
}

void If::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	condition->analyse(analyser, Type::UNKNOWN);
	then->analyse(analyser, req_type);

	if (elze != nullptr) {

		elze->analyse(analyser, req_type);

		if (req_type == Type::VOID) {
			type = Type::VOID;
		} else if (then->type == Type::VOID) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::VOID) { // elze contains return instruction
			type = then->type;
		} else {
			type = Type::get_compatible_type(then->type, elze->type);
		}
		if (then->type != type) {
			then->analyse(analyser, type);
		}
		if (elze->type != type) {
			elze->analyse(analyser, type);
		}
	} else {

		if (req_type == Type::VOID) {
			type = Type::VOID;
		} else {
			type = Type::POINTER; // Pointer because the else will give null
		}
		then->analyse(analyser, type);
	}

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
}

jit_value_t If::compile(Compiler& c) const {

	jit_value_t res = nullptr;
	if (type != Type::VOID) {
		res = jit_value_create(c.F, VM::get_jit_type(type));
	}

	jit_label_t label_else = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	jit_value_t cond = condition->compile(c);

	if (condition->type.nature == Nature::POINTER) {
		jit_value_t cond_bool = VM::is_true(c.F, cond);
		if (condition->type.must_manage_memory()) {
			VM::delete_temporary(c.F, cond);
		}
		jit_insn_branch_if_not(c.F, cond_bool, &label_else);
	} else {
		jit_insn_branch_if_not(c.F, cond, &label_else);
	}

	jit_value_t then_v = then->compile(c);
	if (then_v) {
		jit_insn_store(c.F, res, then_v);
	}
	jit_insn_branch(c.F, &label_end);

	jit_insn_label(c.F, &label_else);

	if (elze != nullptr) {
		jit_value_t else_v = elze->compile(c);
		if (else_v) {
			jit_insn_store(c.F, res, else_v);
		}
	} else {
		if (type != Type::VOID) {
			jit_insn_store(c.F, res, VM::get_null(c.F));
		}
	}

	jit_insn_label(c.F, &label_end);

	return res;
}

}
