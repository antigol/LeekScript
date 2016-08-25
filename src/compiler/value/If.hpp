#ifndef IF_HPP_
#define IF_HPP_

#include "../../compiler/value/Expression.hpp"
#include "../value/Block.hpp"

namespace ls {

class If : public Value {
public:

	Value* condition;
	Block* then;
	Block* elze;

	If();
	virtual ~If();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
