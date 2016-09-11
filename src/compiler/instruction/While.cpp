#include "While.hpp"
#include "../jit/jit_general.hpp"

using namespace std;

namespace ls {

While::While() {
	condition = nullptr;
	body = nullptr;
}

While::~While() {
	delete condition;
	delete body;
}

void While::print(ostream& os, int indent, bool debug) const {
	os << "while ";
	condition->print(os, indent + 1, debug);
	os << " ";
	body->print(os, indent, debug);
}

unsigned While::line() const
{
	return 0;
}

// DONE 2
void While::analyse_help(SemanticAnalyser* analyser)
{
	condition->analyse(analyser);
	if (!Type::intersection(condition->type, Type::LOGIC, &condition->type)) {
		add_error(analyser, SemanticException::MUST_BE_LOGIC_TYPE);
	}

	analyser->enter_loop();
	body->analyse(analyser);
	analyser->leave_loop();
	type = Type::VOID;
}

void While::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	condition->reanalyse(analyser, Type::UNKNOWN);
	body->reanalyse(analyser, Type::VOID);
}

void While::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	condition->finalize(analyser, Type::UNKNOWN);
	body->finalize(analyser, Type::VOID);
}

jit_value_t While::compile(Compiler& c) const {

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// condition
	jit_value_t cond = jit_general::is_true_delete_temporary(c.F, condition->compile(c), condition->type);
	jit_insn_branch_if_not(c.F, cond, &label_end);

	// body
	c.enter_loop(&label_end, &label_cond);
	body->compile(c);
	c.leave_loop();

	// jump to cond
	jit_insn_branch(c.F, &label_cond);

	// end label:
	jit_insn_label(c.F, &label_end);


	return nullptr;
}

}
