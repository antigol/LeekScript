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
	bool ternary;

	If(bool ternary = false);
	virtual ~If();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
