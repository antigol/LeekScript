#ifndef PREFIXEXPRESSION_HPP
#define PREFIXEXPRESSION_HPP

#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
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

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
