#ifndef NULLL_HPP
#define NULLL_HPP

#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Nulll : public Value {
public:

	std::shared_ptr<Token> token;

	Nulll(std::shared_ptr<Token> token);

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
