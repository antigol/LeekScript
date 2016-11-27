#ifndef NULLL_HPP
#define NULLL_HPP

#include "../../compiler/value/Value.hpp"

namespace ls {

class Nulll : public Value {
public:

	Nulll();
	virtual ~Nulll();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
