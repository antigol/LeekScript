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

	Expression();
	Expression(Value*);
	virtual ~Expression();

	void append(Operator*, Value*);

	void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
