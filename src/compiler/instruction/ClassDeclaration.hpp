#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "../../compiler/instruction/Instruction.hpp"
#include "../../compiler/instruction/VariableDeclaration.hpp"
#include "../../compiler/lexical/Ident.hpp"

namespace ls {

class ClassDeclaration : public Instruction {
public:

	std::string name;
	std::vector<VariableDeclaration*> fields;

	ClassDeclaration();
	virtual ~ClassDeclaration();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
