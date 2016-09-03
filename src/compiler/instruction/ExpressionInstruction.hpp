#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/value/Expression.hpp"

namespace ls {

class ExpressionInstruction : public Instruction {
public:

	Value* value;

	ExpressionInstruction(Value* expression);
	virtual ~ExpressionInstruction();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& type) override;
	virtual void preanalyse(SemanticAnalyser*) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
