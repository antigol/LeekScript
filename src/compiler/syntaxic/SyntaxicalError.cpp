#include "../../compiler/syntaxic/SyntaxicalError.hpp"
#include <iostream>
using namespace std;

namespace ls {

SyntaxicalError::SyntaxicalError(const SyntaxicalError& other) {
	this->token = other.token;
	this->type = other.type;
	this->parameters = other.parameters;
}

SyntaxicalError::SyntaxicalError(Token* token, Type type, std::vector<std::string> parameters) {
	this->token = token;
	this->type = type;
	this->parameters = parameters;
}

SyntaxicalError::~SyntaxicalError() {}

std::string SyntaxicalError::message() const {
	return build_message(type, {});
}

std::string SyntaxicalError::build_message(Type type, std::vector<std::string>) {
	return std::string("syntaxical_error<") + std::to_string((int) type) + std::string(">");
}

}
