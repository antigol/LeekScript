#ifndef IDENT_HPP_
#define IDENT_HPP_

#include "../../compiler/lexical/Token.hpp"

namespace ls {

class Ident {
public:

	Token* token;

	Ident(Token* token);
	virtual ~Ident();
};

}

#endif
