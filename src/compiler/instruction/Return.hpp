#ifndef RETURN_HPP
#define RETURN_HPP

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/value/Function.hpp"

namespace ls {

class Return : public Instruction {
public:

	Value* expression;
	Function* function;
	bool in_function;

	Return(Value* = nullptr);
	virtual ~Return();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
