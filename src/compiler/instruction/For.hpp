#ifndef FOR_HPP
#define FOR_HPP

#include <vector>

#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "Instruction.hpp"

namespace ls {

class Block;
class SemanticVar;

class For : public Instruction {
public:

	std::vector<Instruction*> inits;
	Value* condition;
	std::vector<Instruction*> increments;
	Block* body = nullptr;

	For();
	virtual ~For();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
