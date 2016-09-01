#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "LeftValue.hpp"
#include "Value.hpp"

namespace ls {

class PostfixExpression : public Value {
public:

	LeftValue* expression;
	Operator* operatorr;

	PostfixExpression();
	virtual ~PostfixExpression();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
