#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>

#include "../../compiler/lexical/Operator.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class Expression : public Value {
public:

	Value* v1;
	Value* v2;
	Operator* op;

	bool store_result_in_v1;
	bool no_op;
	int operations;
	Type conversion = Type::UNKNOWN;
	void* operator_fun = nullptr;
	bool is_native_method = false;
	Type v1_type;
	Type v2_type;
	Type return_type;

	Expression();
	Expression(Value*);
	virtual ~Expression();

	void append(Operator*, Value*);

	void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
