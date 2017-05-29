#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include "../instruction/Instruction.hpp"
#include "Value.hpp"

namespace ls {

class Block : public Value {
public:

	std::vector<Instruction*> instructions;
	bool temporary_mpz = false;

	Block();
	virtual ~Block();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
