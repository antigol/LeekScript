#include "If.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../jit/jit_general.hpp"

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

// DONE 2
void If::analyse_help(SemanticAnalyser* analyser)
{
	condition->analyse(analyser);
	if (!Type::intersection(condition->type, Type::LOGIC, &condition->type)) {
		add_error(analyser, SemanticException::MUST_BE_LOGIC_TYPE);
	}

	then->analyse(analyser);

	if (elze) {
		elze->analyse(analyser);

		if (then->type == Type::UNREACHABLE) {
			type = elze->type;
		} else if (elze->type == Type::UNREACHABLE) {
			type = then->type;
		} else {
			if (!Type::intersection(then->type, elze->type, &type)) {
				type = Type::VOID;
			}
		}
	} else {
		type = Type::VOID;
	}
}

void If::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	condition->reanalyse(analyser, Type::UNKNOWN);

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

redo:
	then->reanalyse(analyser, type);
	if (elze) {
		elze->reanalyse(analyser, type);

		if (then->type == Type::UNREACHABLE) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::UNREACHABLE) { // elze contains return instruction
			type = then->type;
		} else {
			Type old_type = type;
			if (!Type::intersection(then->type, elze->type, &type)) {
				type = Type::VOID;
			} else if (old_type != type && analyser->errors.empty()) {
				goto redo;
			}
		}
	}
}

void If::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	condition->finalize(analyser, Type::UNKNOWN);

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	then->finalize(analyser, type);

	if (elze) {
		if (then->type == Type::UNREACHABLE) {
			elze->finalize(analyser, type);
			type = elze->type;
		} else {
			type = then->type;
			elze->finalize(analyser, type);
		}
	} else {
		type = Type::VOID;
	}

	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t If::compile(Compiler& c) const {

	jit_value_t res = nullptr;
	if (type != Type::VOID && type != Type::UNREACHABLE) {
		res = jit_value_create(c.F, type.jit_type());
	}

	jit_label_t label_else = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	jit_value_t cond = jit_general::is_true_delete_temporary(c.F, condition->compile(c), condition->type);

	jit_insn_branch_if_not(c.F, cond, &label_else);

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
