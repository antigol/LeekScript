#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Number : public Value {
public:
	std::string value;
	Token* token;

	Number(const std::string& value, Token* token);
	virtual ~Number();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
