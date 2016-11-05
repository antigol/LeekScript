#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "../../compiler/lexical/Token.hpp"

namespace ls {

class Operator {
public:

	TokenType type;
	std::string character;
	int priority;
	bool reversed;

	Operator(Token* token);
	virtual ~Operator();

	void print(std::ostream&);
};

}

#endif
