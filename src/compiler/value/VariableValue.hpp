#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include "LeftValue.hpp"
#include "../Compiler.hpp"

namespace ls {

class Value;
class SemanticVar;

class VariableValue : public LeftValue {
public:

	std::string name;
	Token* token;
	SemanticVar* var;

	VariableValue(Token* token);
	virtual ~VariableValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
	virtual jit_value_t compile_l(Compiler&) const override;
};

}

#endif
