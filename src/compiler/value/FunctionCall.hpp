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

	Type req_method_type;
	Type res_method_type;
	std::vector<Method*> methods;

	FunctionCall();
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
