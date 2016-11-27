#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>

#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"
#include "../../vm/value/LSString.hpp"

namespace ls {

class String : public Value {
public:

	std::string value;
	Token* token;
	LSString* ls_string;

	String(std::string& value, Token* token);
	virtual ~String();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif
