#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

#include "../../compiler/lexical/TokenType.hpp"

namespace ls {

class Token {

public:

	TokenType type;
	std::string content;
	unsigned character;
	unsigned line;
	unsigned size;

	Token(std::string content);
	Token(TokenType type, unsigned int line, unsigned int character, std::string content);
	virtual ~Token();
};

std::ostream& operator << (std::ostream& os, Token& var);

}

#endif
