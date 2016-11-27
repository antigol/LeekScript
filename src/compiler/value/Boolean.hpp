#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "../../compiler/value/Value.hpp"


namespace ls {

class Boolean : public Value {
public:

	bool value;

	Boolean(bool value);
	virtual ~Boolean();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
