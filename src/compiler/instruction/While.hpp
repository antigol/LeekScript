#ifndef WHILE_HPP
#define WHILE_HPP

#include <vector>

#include "../../compiler/lexical/Ident.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"
class SemanticVar;

namespace ls {

class While : public Instruction {
public:

	Value* condition;
	Block* body;

	While();
	virtual ~While();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
