#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include <vector>
#include "Instruction.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Expression.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class SemanticVar;

class VariableDeclaration : public Instruction {
public:

	bool global;
	bool constant;
	std::vector<std::shared_ptr<Token>> variables;
	std::vector<Value*> expressions;
	std::map<std::string, std::shared_ptr<SemanticVar>> vars;
	std::shared_ptr<Token> keyword;

	VariableDeclaration();
	virtual ~VariableDeclaration();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
