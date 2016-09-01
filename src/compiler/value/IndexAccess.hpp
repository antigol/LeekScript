#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "LeftValue.hpp"
#include "Value.hpp"

namespace ls {

class IndexAccess : public LeftValue {
public:

	Value* container;
	Value* key;
	Value* key2;

	IndexAccess();
	virtual ~IndexAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
	virtual jit_value_t compile_l(Compiler&) const override;
};

}

#endif
