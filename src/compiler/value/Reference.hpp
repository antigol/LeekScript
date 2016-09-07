#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class SemanticVar;

class Reference : public Value {
public:

	Token* variable;
	SemanticVar* var;

	Reference();
	Reference(Token* variable);
	virtual ~Reference();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
