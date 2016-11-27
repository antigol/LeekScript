#include "For.hpp"

#include "../instruction/Return.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

For::For() {}

For::~For() {
	for (Instruction* ins : inits) delete ins;
	delete condition;
	for (Instruction* ins : increments) delete ins;
	delete body;
}

void For::print(ostream& os, int indent, bool debug) const {
	os << "for";
	for (Instruction* ins : inits) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << "; ";
	if (condition != nullptr) {
		condition->print(os, indent + 1, debug);
	}
	os << ";";
	for (Instruction* ins : increments) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << " ";
	body->print(os, indent, debug);
}

void For::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (req_type.raw_type == RawType::ARRAY && req_type.nature == Nature::POINTER) {
		type = req_type;
	} else {
		type = Type::VOID;
	}

	analyser->enter_block();

	// Init
	for (Instruction* ins : inits) {
		ins->analyse(analyser, Type::UNKNOWN);
		if (dynamic_cast<Return*>(ins)) {
			type = ins->type;
			analyser->leave_block();
			return;
		}
	}

	// Condition
	if (condition != nullptr) {
		condition->analyse(analyser, Type::UNKNOWN);
	}

	// Body
	analyser->enter_loop();
	if (type == Type::VOID) {
		body->analyse(analyser, Type::VOID);
	} else {
		body->analyse(analyser, type.getElementType());
		type.setElementType(body->type);
	}
	analyser->leave_loop();

	// Increment
	analyser->enter_block();
	for (Instruction* ins : increments) {
		ins->analyse(analyser, Type::VOID);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	analyser->leave_block();

	analyser->leave_block();
}

Compiler::value For::compile(Compiler& c) const {

	c.enter_block(); // { for init ; cond ; inc { body } }<-- this block

	jit_value_t output_v = nullptr;
	if (type.raw_type == RawType::ARRAY && type.nature == Nature::POINTER) {
		output_v = VM::create_array(c.F, type.getElementType());
		VM::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_inc = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// Init
	for (Instruction* ins : inits) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			jit_value_t return_v = VM::clone_obj(c.F, output_v);
			c.leave_block(c.F);
			return {return_v, type};
		}
	}

	// Cond
	jit_insn_label(c.F, &label_cond);
	VM::inc_ops(c.F, 1);
	if (condition != nullptr) {
		auto condition_v = condition->compile(c);
		if (condition->type.nature == Nature::POINTER) {
			jit_value_t bool_v = VM::is_true(c.F, condition_v.v);

			if (condition->type.must_manage_memory()) {
				VM::delete_temporary(c.F, condition_v.v);
			}
			jit_insn_branch_if_not(c.F, bool_v, &label_end);
		} else {
			jit_insn_branch_if_not(c.F, condition_v.v, &label_end);
		}
	}

	// Body
	c.enter_loop(&label_end, &label_inc);
	auto body_v = body->compile(c);
	if (output_v && body_v.v) {
		// transfer the ownership of the temporary variable `body_v`
		VM::push_move_array(c.F, type.getElementType(), output_v, body_v.v);
	}
	c.leave_loop();
	jit_insn_label(c.F, &label_inc);

	// Inc
	c.enter_block();
	for (Instruction* ins : increments) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	c.leave_block(c.F);
	jit_insn_branch(c.F, &label_cond);

	// End
	jit_insn_label(c.F, &label_end);
	jit_value_t return_v = VM::clone_obj(c.F, output_v);
	c.leave_block(c.F);
	return {return_v, type};
}

}
