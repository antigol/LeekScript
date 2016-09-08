#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <vector>
#include "Value.hpp"

namespace ls {

class Block : public Value {
public:

	std::vector<Value*> instructions;
	Function* function; // if the block is the main block of a functi<on

	Block();
	virtual ~Block();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	jit_value_t compile(Compiler&) const;

};

}

#endif
