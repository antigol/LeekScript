#ifndef BREAK_HPP
#define BREAK_HPP

#include "../value/Value.hpp"

namespace ls {

class Break : public Value {
public:

	int deepness;

	Break();
	virtual ~Break();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
