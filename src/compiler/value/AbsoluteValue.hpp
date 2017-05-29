#ifndef ABSOLUTEVALUE_HPP
#define ABSOLUTEVALUE_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class AbsoluteValue : public Value {
public:

	Value* expression;
	std::shared_ptr<Token> open_pipe;
	std::shared_ptr<Token> close_pipe;

	AbsoluteValue();
	virtual ~AbsoluteValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
