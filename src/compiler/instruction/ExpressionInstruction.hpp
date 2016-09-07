#ifndef EXPRESSIONINSTRUCTION_HPP
#define EXPRESSIONINSTRUCTION_HPP

#include "../value/Value.hpp"

namespace ls {

class ExpressionInstruction : public Value {
public:

	Value* value;

	ExpressionInstruction(Value* expression);
	virtual ~ExpressionInstruction();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
