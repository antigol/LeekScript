#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>

#include "../value/Value.hpp"
#include "../TypeName.hpp"

namespace ls {

class SemanticVar;

class VariableDeclaration : public Value {
public:

	bool global;
	Token* variable;
	TypeName* typeName;
	Value* expression;
	SemanticVar* var;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
