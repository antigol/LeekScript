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

void If::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	condition->analyse(analyser, Type::UNKNOWN);
	if (condition->type == Type::FUNCTION || condition->type == Type::VOID) {
		stringstream oss;
		condition->print(oss);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, condition->line(), oss.str() });
	}

	if (elze) {
		then->preanalyse(analyser);
		elze->preanalyse(analyser);

		if (then->type == Type::UNREACHABLE && elze->type == Type::UNREACHABLE) {
			type = Type::UNREACHABLE;
			then->analyse(analyser, Type::UNKNOWN);
			elze->analyse(analyser, Type::UNKNOWN);
			return;
		} else if (then->type == Type::UNREACHABLE) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::UNREACHABLE) { // elze contains return instruction
			type = then->type;
		} else {
			if (!Type::get_intersection(then->type, elze->type, &type)) type = Type::VOID;

#if DEBUG > 0
		cout << "#If " << then->type << " + " << elze->type << " = " << type << endl;
#endif
		}

		if (!Type::get_intersection(type, req_type, &type)) {
			stringstream oss;
			print(oss, 0, false);
			analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
		}
		type.make_it_complete();

		then->analyse(analyser, type);
		elze->analyse(analyser, type);
	} else {
		then->analyse(analyser, Type::VOID);
		type = Type::VOID;

		if (!Type::get_intersection(type, req_type)) {
			stringstream oss;
			print(oss, 0, false);
			analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
		}
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

void If::preanalyse(SemanticAnalyser* analyser)
{
	condition->preanalyse(analyser);

	if (elze) {
		then->preanalyse(analyser);
		elze->preanalyse(analyser);

		if (then->type == Type::UNREACHABLE) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::UNREACHABLE) { // elze contains return instruction
			type = then->type;
		} else {
			if (!Type::get_intersection(then->type, elze->type, &type)) type = Type::VOID;
		}
	} else {
		then->preanalyse(analyser);
		type = Type::VOID;
	}
}

jit_value_t If::compile(Compiler& c) const {

	jit_value_t res = nullptr;
	if (type != Type::VOID) {
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
