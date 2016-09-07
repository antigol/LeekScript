#include "If.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

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

void If::preanalyse(SemanticAnalyser* analyser)
{
	condition->preanalyse(analyser);
	condition->will_require(analyser, Type::LOGIC);

	if (elze) {
		then->preanalyse(analyser);
		elze->preanalyse(analyser);

		if (then->type == Type::UNREACHABLE) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::UNREACHABLE) { // elze contains return instruction
			type = then->type;
		} else {
			if (!Type::intersection(then->type, elze->type, &type)) {
				type = Type::VOID;
			} else {
				then->will_require(analyser, type);
				elze->will_require(analyser, type);
			}
		}
	} else {
		then->preanalyse(analyser);
		type = Type::VOID;
	}
}

void If::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	then->will_require(analyser, type);
	if (elze) elze->will_require(analyser, type);
}

void If::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	condition->analyse(analyser, Type::UNKNOWN);

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();

	then->analyse(analyser, type);

	if (elze) {
		elze->analyse(analyser, type);
	}
}

jit_value_t If::compile(Compiler& c) const {

	jit_value_t res = nullptr;
	if (type.raw_type->nature() != Nature::VOID) {
		res = jit_value_create(c.F, type.jit_type());
	}

	jit_label_t label_else = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	jit_value_t cond = condition->compile(c);

	if (condition->type.raw_type->nature() == Nature::LSVALUE) {
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

	if (elze) {
		jit_value_t else_v = elze->compile(c);
		if (else_v) {
			jit_insn_store(c.F, res, else_v);
		}
	}

	jit_insn_label(c.F, &label_end);

	return res;
}

}
