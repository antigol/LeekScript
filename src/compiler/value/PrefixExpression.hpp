#ifndef PREFIXEXPRESSION_HPP
#define PREFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "Value.hpp"
#include "../lexical/Operator.hpp"

namespace ls {

class PrefixExpression : public Value {
public:

	Operator* operatorr;
	Value* expression;

	PrefixExpression();
	virtual ~PrefixExpression();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
