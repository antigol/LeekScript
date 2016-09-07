#ifndef NULLL_HPP
#define NULLL_HPP

#include "../../compiler/value/Value.hpp"

namespace ls {

class Nulll : public Value {
public:

	Nulll();
	virtual ~Nulll();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
