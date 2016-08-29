#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../lexical/Ident.hpp"
#include "../../vm/VM.hpp"
#include "../TypeName.hpp"

namespace ls {

class SemanticVar;

class VariableDeclaration : public Instruction {
public:

	bool global;
	Token* variable;
	TypeName* typeName;
	Value* expression;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
