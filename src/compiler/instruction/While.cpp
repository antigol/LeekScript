#include "../../compiler/instruction/While.hpp"

#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

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

void While::analyse(SemanticAnalyser* analyser, const Type&) {

	condition->analyse(analyser, Type::UNKNOWN);
	if (condition->type.nature != Nature::LSVALUE
			&& condition->type.raw_type != RawType::BOOLEAN
			&& condition->type.raw_type != RawType::I32
			&& condition->type.raw_type != RawType::I64) {
		analyser->add_error({ SemanticException::TYPE_MISMATCH });
	}
	analyser->enter_loop();
	body->analyse(analyser, Type::VOID);
	analyser->leave_loop();
}

jit_value_t While::compile(Compiler& c) const {

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// condition
	jit_value_t cond = condition->compile(c);
	if (condition->type.nature == Nature::LSVALUE) {
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
