#include "../../compiler/lexical/Token.hpp"

#include <iostream>

using namespace std;

namespace ls {

Token::Token(TokenType type, size_t raw, size_t line, size_t character, string content) : location({line, character - content.size() - 1, raw - content.size() - 1}, {line, character, raw - 1}) {

	this->type = type;
	this->content = string(content);

	if (type == TokenType::STRING) {
		this->location.start.column--;
		this->location.start.raw--;
		this->location.end.raw++;
		this->location.end.column++;
		this->size = content.size() + 2;
	} else {
		this->size = content.size();
	}
}

}
