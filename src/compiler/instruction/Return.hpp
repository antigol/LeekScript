#ifndef RETURN_HPP
#define RETURN_HPP

#include "../value/Value.hpp"

namespace ls {

class Return : public Value {
public:

	Value* expression;

	Return();
	Return(Value*);
	virtual ~Return();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
