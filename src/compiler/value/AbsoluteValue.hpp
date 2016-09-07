#ifndef ABSOLUTEVALUE_HPP
#define ABSOLUTEVALUE_HPP

#include "../../compiler/value/Expression.hpp"

namespace ls {

class AbsoluteValue : public Value {
public:

	Value* expression;

	AbsoluteValue();
	virtual ~AbsoluteValue();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
