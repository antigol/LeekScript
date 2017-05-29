#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>

#include "../../compiler/lexical/Ident.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class Foreach : public Instruction {
public:

	std::shared_ptr<Token> key;
	std::shared_ptr<Token> value;
	Value* container;
	Block* body;
	
	Type key_type;
	Type value_type;
	std::shared_ptr<SemanticVar> value_var;
	std::shared_ptr<SemanticVar> key_var;

	Foreach();
	virtual ~Foreach();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
