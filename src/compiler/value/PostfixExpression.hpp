#ifndef POSTFIXEXPRESSION_HPP
#define POSTFIXEXPRESSION_HPP

#include "Expression.hpp"
#include "LeftValue.hpp"
#include "Value.hpp"

namespace ls {

class PostfixExpression : public Value {
public:

	LeftValue* expression;
	std::shared_ptr<Operator> operatorr;
	bool return_value;

	PostfixExpression();
	virtual ~PostfixExpression();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
