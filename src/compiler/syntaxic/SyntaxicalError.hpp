#ifndef SYNTAXICALERROR_HPP
#define SYNTAXICALERROR_HPP

#include <string>
#include "../lexical/Token.hpp"

namespace ls {

class SyntaxicalError {
public:

	Token* token;
	std::string message;

	SyntaxicalError() = delete;
	SyntaxicalError(SyntaxicalError&);
	SyntaxicalError(Token* token, std::string message);
	virtual ~SyntaxicalError();
};

}

#endif
