#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "../../compiler/value/Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Number : public Value {
public:

	Token* token;
	std::string value;
	std::string clean_value;
	int base = 10;
	int int_value = 0;
	long long_value = 0;
	double double_value = 0;
	mpz_t mpz_value;
	bool mpz_value_initialized = false;

	Number(std::string value, Token* token);
	virtual ~Number();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
