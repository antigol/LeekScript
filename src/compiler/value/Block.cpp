#include "Block.hpp"

#include "../instruction/Return.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

Block::Block() {
}

Block::~Block() {
	for (Value* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, int indent, bool debug) const {
	os << "{";
	os << endl;
	for (Value* instruction : instructions) {
		os << tabs(indent + 1);
		instruction->print(os, indent + 1, debug);
		os << endl;
	}
	os << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Block::line() const {
	return 0;
}

void Block::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	analyser->enter_block();

	for (size_t i = 0; i < instructions.size(); ++i) {
		Value* ins = instructions[i];

		if (dynamic_cast<Return*>(ins)) {
			ins->analyse(analyser, Type::UNKNOWN);
			type = Type::UNREACHABLE;
			analyser->leave_block();
			return; // no need to compile after a return
		}

		if (i == instructions.size() - 1) {
			// last instruction
			ins->analyse(analyser, req_type);
			type = ins->type;
			analyser->leave_block();
			return;
		} else {
			ins->analyse(analyser, Type::VOID);
		}
	}

	// empty block
	type = Type::VOID;
	if (!type.match_with_generic(req_type)) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}

	analyser->leave_block();
	assert(type.is_complete() || !analyser->errors.empty());
}

void Block::preanalyse(SemanticAnalyser* analyser)
{
	analyser->enter_block();

	for (size_t i = 0; i < instructions.size(); ++i) {
		Value* ins = instructions[i];

		if (dynamic_cast<Return*>(ins)) {
			ins->preanalyse(analyser);
			type = Type::UNREACHABLE;
			analyser->leave_block();
			return; // no need to compile after a return
		}

		if (i == instructions.size() - 1) {
			// last instruction
			ins->preanalyse(analyser);
			type = ins->type;
			analyser->leave_block();
			return;
		} else {
			ins->preanalyse(analyser);
		}
	}

	// empty block
	type = Type::VOID;
	analyser->leave_block();
}

LSValue* Block_move(LSValue* value) {
	/* Move the value if it's a temporary variable
	 * or if it's only attached to the current block.
	 */
	if (value == nullptr) return nullptr;
	if (value->refs <= 1) {
		value->refs = 0;
		return value;
	}
	return value->clone();
}


jit_value_t Block::compile(Compiler& c) const {

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {
		jit_value_t val = instructions[i]->compile(c);
		if (dynamic_cast<Return*>(instructions[i])) {
			break; // no need to compile after a return
		}
		if (i == instructions.size() - 1 && instructions[i]->type.raw_type.nature() != Nature::VOID) {
			if (type.must_manage_memory()) {
				jit_type_t args[1] = { LS_POINTER };
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
				jit_value_t ret = jit_insn_call_native(c.F, "true_move", (void*) Block_move, sig, &val, 1, JIT_CALL_NOTHROW);
				c.leave_block(c.F);
				return ret;
			} else {
				c.leave_block(c.F);
				return val;
			}
		}
	}
	c.leave_block(c.F);

	if (type == Type::VAR) {
		return VM::create_null(c.F);
	}
	return nullptr;
}

}
