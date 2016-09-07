#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP

#include "Value.hpp"


namespace ls {

class Boolean : public Value {
public:

	bool value;

	Boolean(bool value);
	virtual ~Boolean();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
