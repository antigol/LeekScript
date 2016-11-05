#include "../../compiler/lexical/Token.hpp"

#include <iostream>

using namespace std;

namespace ls {

Token::Token(std::string content) {
	character = -1;
	line = -1;
	type = TokenType::UNKNOW;
	this->content = content;
	size = 0;
}

Token::Token(TokenType type, unsigned int line, unsigned int character, string content) {

	this->type = type;
	this->character = character - content.size() - 1;
	this->line = line;
	this->content = string(content);

	if (type == TokenType::STRING) {
		this->character--;
		this->size = content.size() + 2;
	} else {
		this->size = content.size();
	}
}

Token::~Token() {}

}
