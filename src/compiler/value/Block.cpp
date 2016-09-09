#include "Block.hpp"

#include "../instruction/Return.hpp"
#include "Function.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;

namespace ls {

Block::Block() {
	function = nullptr;
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

// DONE 2
void Block::analyse_help(SemanticAnalyser* analyser)
{
	analyser->enter_block(this);

	// Void if empty
	type = Type::VOID;

	for (size_t i = 0; i < instructions.size(); ++i) {
		Value* ins = instructions[i];

		if (i == instructions.size() - 1) {
			// last instruction
			ins->analyse(analyser);
			type = ins->type;
		} else {
			ins->analyse(analyser);
			if (ins->type == Type::UNREACHABLE) {
				type = Type::UNREACHABLE;
				break;
			}
		}
	}

	analyser->leave_block();
}

void Block::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	// tip! Intersection of any type with UNREACHABLE gives UNREACHABLE
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	for (size_t i = 0; i < instructions.size(); ++i) {
		Value* ins = instructions[i];

		if (i == instructions.size() - 1) {
			// last instruction
			if (function) {
				ins->reanalyse(analyser, function->type.return_type());
				if (ins->type != Type::UNREACHABLE) {
					if (!Type::intersection(function->type.return_types[0], ins->type, &function->type.return_types[0])) {
						add_error(analyser, SemanticException::TYPE_MISMATCH);
					}
				}
			} else {
				ins->reanalyse(analyser, type);
			}
			type = ins->type;
		} else {
			ins->reanalyse(analyser, Type::VOID);
			if (ins->type == Type::UNREACHABLE) {
				type = Type::UNREACHABLE;
				break;
			}
		}
	}

	if (instructions.empty() && function) {
		function->type.set_return_type(type);
	}

}

void Block::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	// tip! Intersection of any type with UNREACHABLE gives UNREACHABLE
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	for (size_t i = 0; i < instructions.size(); ++i) {
		Value* ins = instructions[i];

		if (i == instructions.size() - 1) {
			// last instruction
			if (function) {
				ins->finalize(analyser, function->type.return_type());
				if (ins->type != Type::UNREACHABLE) {
					function->type.set_return_type(ins->type);
				}
			} else {
				ins->finalize(analyser, type);
			}
			type = ins->type;
		} else {
			ins->finalize(analyser, Type::VOID);
			if (ins->type == Type::UNREACHABLE) {
				type = Type::UNREACHABLE;
				break;
			}
		}
	}

	assert(type.is_pure() || !analyser->errors.empty());
}



jit_value_t Block::compile(Compiler& c) const {

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {
		jit_value_t val = instructions[i]->compile(c);
		if (instructions[i]->type == Type::UNREACHABLE) {
			break; // no need to compile after a return
		}
		if (i == instructions.size() - 1 && instructions[i]->type.raw_type->nature() != Nature::VOID) {
			if (type.must_manage_memory()) {
				jit_value_t ret = VM::move_obj(c.F, val); // TODO add true move by checking the variable scope
				c.leave_block(c.F);
				return ret;
			} else {
				c.leave_block(c.F);
				return val;
			}
		}
	}
	c.leave_block(c.F);

	return nullptr;
}

}
