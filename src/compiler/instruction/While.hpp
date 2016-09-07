#ifndef WHILE_HPP
#define WHILE_HPP

#include <vector>

#include "../value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class While : public Value {
public:

	Value* condition;
	Block* body;

	While();
	virtual ~While();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
