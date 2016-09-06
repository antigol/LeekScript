#ifndef CONTINUE_HPP
#define CONTINUE_HPP

#include "../value/Value.hpp"

namespace ls {

class Continue : public Value {
public:

	int deepness;

	Continue();
	virtual ~Continue();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif

