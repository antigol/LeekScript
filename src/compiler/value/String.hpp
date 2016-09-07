#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>

#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class String : public Value {
public:

	std::string value;
	Token* token;

	String(std::string& value, Token* token);
	virtual ~String();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
