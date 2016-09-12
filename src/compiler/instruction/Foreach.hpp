#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>

#include "../lexical/Ident.hpp"
#include "../value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class Foreach : public Value {
public:

	Token* key;
	Token* value;
	Value* container;
	Block* body;

	SemanticVar* value_var;
	SemanticVar* key_var;

	Foreach();
	virtual ~Foreach();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
