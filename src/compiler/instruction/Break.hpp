#ifndef BREAK_HPP
#define BREAK_HPP

#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

class Break : public Instruction {
public:

	int deepness;

	Break();
	virtual ~Break();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
