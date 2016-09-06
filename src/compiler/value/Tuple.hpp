#ifndef TUPLE_H
#define TUPLE_H

#include "Value.hpp"

namespace ls {

class Tuple : public Value
{
public:
	std::vector<Value*> elements;

	Tuple();

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const override;
	virtual unsigned line() const override;
	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual jit_value_t compile(Compiler&) const override;
};

}
#endif // TUPLE_H
