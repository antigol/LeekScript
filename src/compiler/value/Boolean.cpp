#include "Boolean.hpp"

#include "../../vm/VM.hpp"
#include "../../vm/value/LSBoolean.hpp"

using namespace std;

namespace ls {

Boolean::Boolean(std::shared_ptr<Token> token) {
	this->token = token;
	this->value = token->type == TokenType::TRUE;
	type = Type::BOOLEAN;
	constant = true;
}

void Boolean::print(std::ostream& os, int, bool debug) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << type;
	}
}

Location Boolean::location() const {
	return token->location;
}

void Boolean::analyse(SemanticAnalyser*, const Type& req_type) {
	if (req_type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
}

Compiler::value Boolean::compile(Compiler& c) const {
	if (type.nature == Nature::POINTER) {
		auto b = LSBoolean::get(value);
		return {c.new_pointer(b).v, Type::BOOLEAN_P};
	} else {
		return {LS_CREATE_BOOLEAN(c.F, value), Type::BOOLEAN};
	}
}

Value* Boolean::clone() const {
	auto b = new Boolean(token);
	b->value = value;
	return b;
}

}
