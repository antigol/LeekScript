#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <vector>

#include "Value.hpp"

namespace ls {

class Array : public Value {
public:

	std::vector<Value*> expressions;
	bool interval = false;

	Array();
	virtual ~Array();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
