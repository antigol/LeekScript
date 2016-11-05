#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>

#include "../../compiler/value/Value.hpp"

namespace ls {

class FunctionCall : public Value {
public:

	Value* function;
	std::vector<Value*> arguments;

	bool is_native = false;
	std::string native_func;
	Type return_type;
	void* std_func;
	Value* this_ptr;
	bool is_native_method = false;

	FunctionCall();
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
