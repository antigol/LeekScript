#include "Block.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../instruction/Return.hpp"
#include "../instruction/Throw.hpp"

using namespace std;

namespace ls {

Block::Block() {
	type = Type::VOID;
}

Block::~Block() {
	for (Instruction* instruction : instructions) {
		delete instruction;
	}
}

void Block::print(ostream& os, int indent, bool debug) const {
	os << "{";
	os << endl;
	for (auto& instruction : instructions) {
		os << tabs(indent + 1);
		instruction->print(os, indent + 1, debug);
		os << endl;
	}
	os << tabs(indent) << "}";
	if (debug) {
		os << " " << types;
	}
}

Location Block::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Block::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// std::cout << "Block analyse " << req_type << std::endl;

	analyser->enter_block();

	type = Type::VOID;
	types.clear();

	for (unsigned i = 0; i < instructions.size(); ++i) {
		if (i < instructions.size() - 1) {
			// Not the last instruction, it must return void
			instructions[i]->analyse(analyser, Type::VOID);
		} else {
			// Last instruction : must return the required type
			instructions[i]->analyse(analyser, req_type);
			type = instructions[i]->type;
			types.add(instructions[i]->types);
		}
		// A return instruction
		if (dynamic_cast<Return*>(instructions[i]) or dynamic_cast<Throw*>(instructions[i])) {
			type = Type::VOID; // This block has really no type
			analyser->leave_block();
			return; // no need to compile after a return
		}
	}

	analyser->leave_block();

	if (type.nature == Nature::VOID) { // empty block or last instruction type is VOID
		if (req_type.nature != Nature::UNKNOWN) {
			type.nature = req_type.nature;
		} else {
			type = Type::VOID;
		}
	}
	if (type == Type::MPZ) {
		type = Type::MPZ_TMP;
	} else if (type == Type::MPZ_TMP) {
		temporary_mpz = true;
	}
}

Compiler::value Block::compile(Compiler& c) const {

	// std::cout << "Compile block " << type << std::endl;

	c.enter_block();

	for (unsigned i = 0; i < instructions.size(); ++i) {

		auto val = instructions[i]->compile(c);

		if (dynamic_cast<Return*>(instructions[i]) or dynamic_cast<Throw*>(instructions[i])) {
			break; // no need to compile after a return
		}
		if (i == instructions.size() - 1) {
			if (type.must_manage_memory()) {
				auto ret = c.insn_call(type, {val}, +[](LSValue* value) {
					return value->move();
				});
				c.leave_block();
				return ret;
			} else if (type == Type::MPZ_TMP && !temporary_mpz) {
				auto v = c.insn_clone_mpz(val);
				c.leave_block();
				return v;
			} else {
				c.leave_block();
				return val;
			}
		}
	}
	c.leave_block();
	return {nullptr, Type::UNKNOWN};
}

Value* Block::clone() const {
	auto b = new Block();
	for (const auto& i : instructions) {
		b->instructions.push_back(i->clone());
	}
	return b;
}

}
