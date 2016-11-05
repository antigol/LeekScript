#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include "../lexical/Ident.hpp"
#include "../value/LeftValue.hpp"
#include "../Compiler.hpp"

namespace ls {

class Value;
class SemanticVar;
enum class VarScope;

class VariableValue : public LeftValue {
public:

	std::string name;
	Token* token;
	SemanticVar* var;
	int capture_index = 0;
	VarScope scope;

	VariableValue(Token* token);
	virtual ~VariableValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>&) override;
	virtual bool will_store(SemanticAnalyser* analyser, const Type& type) override;
	void must_return(SemanticAnalyser* analyser, const Type& type) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
	virtual jit_value_t compile_l(Compiler&) const override;
};

}

#endif
