#include "../../compiler/syntaxic/SyntaxicalError.hpp"
#include <iostream>
using namespace std;

namespace ls {

SyntaxicalError::SyntaxicalError(SyntaxicalError& other) {
	this->token = other.token;
	this->message = other.message;
}

SyntaxicalError::SyntaxicalError(Token* token, string message) {
	this->token = token;
	this->message = message;
}

SyntaxicalError::~SyntaxicalError() {}

}
