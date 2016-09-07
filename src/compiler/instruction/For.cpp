#include "For.hpp"
#include "Return.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

For::For() {
}

For::~For() {
	for (Value* ins : inits) delete ins;
	delete condition;
	for (Value* ins : increments) delete ins;
	delete body;
}

void For::print(ostream& os, int indent, bool debug) const {
	os << "for";
	for (Value* ins : inits) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << "; ";
	condition->print(os, indent + 1, debug);
	os << ";";
	for (Value* ins : increments) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << " ";
	body->print(os, indent, debug);
}

unsigned For::line() const
{
	return 0;
}

void For::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
}

void For::will_require(SemanticAnalyser* analyser, const Type& req_type)
{

}

void For::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
	if (req_type.raw_type == &RawType::VEC) {
		type = req_type;
	} else {
		type = Type::VOID;
	}

	analyser->enter_block();

	// Init
	for (Value* ins : inits) {
		ins->analyse(analyser, Type::VOID);
		if (dynamic_cast<Return*>(ins)) {
			analyser->leave_block();
			return;
		}
	}

	// Condition
	condition->analyse(analyser, Type::UNKNOWN);
	if (condition->type == Type::FUNCTION || condition->type == Type::VOID) {
		stringstream oss;
		condition->print(oss);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, condition->line(), oss.str() });
	}

	// Body
	analyser->enter_loop();
	if (type == Type::VOID) {
		body->analyse(analyser, Type::VOID);
	} else {
		body->analyse(analyser, type.element_type(0));
		if (type.element_type(0) == Type::UNKNOWN) {
			type.set_element_type(0, body->type);
		} else if (type.element_type(0) != body->type) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}
	analyser->leave_loop();

	// Increment
	analyser->enter_block();
	for (Value* ins : increments) {
		ins->analyse(analyser, Type::VOID);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	analyser->leave_block();

	analyser->leave_block();
	assert(type.is_pure());
}

jit_value_t For::compile(Compiler& c) const {

	c.enter_block(); // { for init ; cond ; inc { body } }<-- this block

	jit_value_t output_v = nullptr;
	if (type.raw_type == &RawType::VEC) {
		output_v = VM::create_vec(c.F, type.element_type(0));
		VM::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_inc = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// Init
	for (Value* ins : inits) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			jit_value_t return_v = VM::clone_obj(c.F, output_v);
			c.leave_block(c.F);
			return return_v;
		}
	}

	// Cond
	jit_insn_label(c.F, &label_cond);
	jit_value_t condition_v = condition->compile(c);
	if (condition->type.raw_type->nature() == Nature::LSVALUE) {
		jit_value_t bool_v = VM::is_true(c.F, condition_v);

		if (condition->type.must_manage_memory()) {
			VM::delete_temporary(c.F, condition_v);
		}

		jit_insn_branch_if_not(c.F, bool_v, &label_end);
	} else {
		jit_insn_branch_if_not(c.F, condition_v, &label_end);
	}

	// Body
	c.enter_loop(&label_end, &label_inc);
	jit_value_t body_v = body->compile(c);
	if (output_v && body_v) {
		// transfer the ownership of the temporary variable `body_v`
		VM::push_move_inc_vec(c.F, type.element_type(0), output_v, body_v);
	}
	c.leave_loop();
	jit_insn_label(c.F, &label_inc);

	// Inc
	c.enter_block();
	for (Value* ins : increments) {
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
	return return_v;
}

}
