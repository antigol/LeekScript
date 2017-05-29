#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"
#include "Function.hpp"

namespace ls {

class FunctionCall : public Value {
public:

	std::shared_ptr<Token> token;
	Value* function;
	std::vector<Value*> arguments;
	std::shared_ptr<Token> closing_parenthesis;

	bool is_native = false;
	std::string native_func;
	Type return_type;
	void* std_func;
	Value* this_ptr;
	bool is_native_method = false;
	bool is_unknown_method = false;
	Value* object = nullptr;
	Function* function_object;
	Type function_type;

	FunctionCall(std::shared_ptr<Token> t);
	virtual ~FunctionCall();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	bool will_take(SemanticAnalyser*, const std::vector<Type>& args, int level);

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
