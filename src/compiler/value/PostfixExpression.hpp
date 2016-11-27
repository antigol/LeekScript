#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP

#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class PostfixExpression : public Value {
public:

	LeftValue* expression;
	Operator* operatorr;
	bool return_value;

	PostfixExpression();
	virtual ~PostfixExpression();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
