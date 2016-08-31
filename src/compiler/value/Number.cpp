#include "Number.hpp"
#include "../../vm/VM.hpp"
#include <climits>
#include <cassert>

using namespace std;

namespace ls {

Number::Number(double value, Token* token) {
	this->value = value;
	this->token = token;
}

Number::~Number() {
}

void Number::print(ostream& os, int, bool debug) const {
	os << value;
	if (debug) {
		os << " " << type;
	}
}

unsigned Number::line() const {
	return token->line;
}

void Number::analyse(SemanticAnalyser*, const Type& req_type) {

	constant = true;

	if (req_type == Type::VAR) {
		type = Type::VAR;
	} else if (req_type == Type::F32 || req_type == Type::F64 || req_type == Type::I32 || req_type == Type::I64) {
		type = req_type;
	} else {
		if (value != (int) value) {
			type = Type::F64;
		} else {
			type = Type::I32;
		}
	}
}

jit_value_t Number::compile(Compiler& c) const {

	if (type == Type::VAR) {
		jit_value_t val = LS_CREATE_F64(c.F, value);
		return VM::value_to_pointer(c.F, val, Type::F64);
	}
	if (type == Type::F64) {
		return LS_CREATE_F64(c.F, value);
	}
	if (type == Type::I32) {
		return LS_CREATE_I32(c.F, value);
	}
	if (type == Type::I64) {
		return LS_CREATE_I64(c.F, value);
	}
	assert(0);
	return nullptr;
}

}


// TODO : convertion done here (useful for i64)
