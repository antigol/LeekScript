#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>

#include "Value.hpp"
#include "../../vm/Module.hpp"

namespace ls {

class FunctionCall : public Value {
public:

	Value* function;
	std::vector<Value*> arguments;

	Method method;

	FunctionCall();
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
