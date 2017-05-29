#include "For.hpp"

#include "../instruction/Return.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

For::For() {}

For::~For() {
	delete condition;
	delete body;
	for (Instruction* ins : inits) {
		delete ins;
	}
	for (Instruction* ins : increments) {
		delete ins;
	}
}

void For::print(ostream& os, int indent, bool debug) const {
	os << "for";
	for (auto ins : inits) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << "; ";
	if (condition != nullptr) {
		condition->print(os, indent + 1, debug);
	}
	os << ";";
	for (auto ins : increments) {
		os << " ";
		ins->print(os, indent + 1, debug);
	}
	os << " ";
	body->print(os, indent, debug);
}

Location For::location() const {
	return {{0, 0, 0}, {0, 0, 0}};
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
		c.insn_inc_refs({output_v, type});
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	jit_label_t label_cond = jit_label_undefined;
	jit_label_t label_inc = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	// Init
	for (Instruction* ins : inits) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			auto return_v = c.clone({output_v, type});
			c.leave_block();
			return return_v;
		}
	}

	// Cond
	jit_insn_label(c.F, &label_cond);
	c.inc_ops(1);
	if (condition != nullptr) {
		auto condition_v = condition->compile(c);
		condition->compile_end(c);
		if (condition->type.nature == Nature::POINTER) {
			auto bool_v = c.insn_to_bool(condition_v);
			c.insn_delete_temporary(condition_v);
			jit_insn_branch_if_not(c.F, bool_v.v, &label_end);
		} else {
			jit_insn_branch_if_not(c.F, condition_v.v, &label_end);
		}
	}

	// Body
	c.enter_loop(&label_end, &label_inc);
	auto body_v = body->compile(c);
	if (output_v && body_v.v) {
		// transfer the ownership of the temporary variable `body_v`
		c.insn_push_array({output_v, type}, body_v);
	}
	c.leave_loop();
	jit_insn_label(c.F, &label_inc);

	// Inc
	c.enter_block();
	for (auto& ins : increments) {
		ins->compile(c);
		if (dynamic_cast<Return*>(ins)) {
			break;
		}
	}
	c.leave_block();
	jit_insn_branch(c.F, &label_cond);

	// End
	jit_insn_label(c.F, &label_end);
	auto return_v = c.clone({output_v, type});
	c.leave_block();
	return return_v;
}

Instruction* For::clone() const {
	auto f = new For();
	for (const auto& i : inits) {
		f->inits.push_back(i->clone());
	}
	f->condition = condition->clone();
	for (const auto& i : increments) {
		f->increments.push_back(i->clone());
	}
	f->body = (Block*) body->clone();
	return f;
}

}
