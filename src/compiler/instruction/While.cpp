#include "While.hpp"

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

// DONE 1
void While::analyse_help(SemanticAnalyser* analyser)
{
	condition->analyse(analyser);
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
	condition->reanalyse(analyser, Type::LOGIC);
	body->reanalyse(analyser, Type::VOID);
}

void While::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	condition->finalize(analyser, Type::LOGIC);
	body->finalize(analyser, Type::VOID);
}

jit_value_t While::compile(Compiler& c) const {

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// condition
	jit_value_t cond = condition->compile(c);
	if (condition->type.raw_type->nature() == Nature::LSVALUE) {
		jit_value_t cond_bool = VM::is_true(c.F, cond);

		if (condition->type.must_manage_memory()) {
			VM::delete_temporary(c.F, cond);
		}

		jit_insn_branch_if_not(c.F, cond_bool, &label_end);
	} else {
		jit_insn_branch_if_not(c.F, cond, &label_end);
	}

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
