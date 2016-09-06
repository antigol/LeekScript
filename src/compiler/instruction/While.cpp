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

void While::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
}

void While::will_require(SemanticAnalyser* analyser, const Type& req_type)
{

}

void While::analyse(SemanticAnalyser* analyser, const Type&) {
	assert(0);

	condition->analyse(analyser, Type::UNKNOWN);
	if (condition->type == Type::FUNCTION || condition->type == Type::VOID) {
		stringstream oss;
		condition->print(oss);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, condition->line(), oss.str() });
	}
	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();

	type = Type::VOID;
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
