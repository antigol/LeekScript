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

void Number::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	preanalyse(analyser);
	if (!type.match_with_generic(req_type, &type)) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}
	type.make_it_complete();
	assert(type.is_complete() || !analyser->errors.empty());
}

void Number::preanalyse(SemanticAnalyser* analyser)
{
	constant = true;
	if (value != (int) value) {
		type = Type(RawType::UNKNOWN, { Type::F64, Type::VAR }); // in preference order
	} else {
		type = Type(RawType::UNKNOWN, { Type::I32, Type::F64, Type::VAR });
	}
}

jit_value_t Number::compile(Compiler& c) const {

	if (type == Type::VAR) {
		jit_value_t val = VM::create_f64(c.F, value);
		return VM::value_to_lsvalue(c.F, val, Type::F64); // TODO create_lsreal
	}
	if (type == Type::F32) {
		return VM::create_f32(c.F, value);
	}
	if (type == Type::F64) {
		return VM::create_f64(c.F, value);
	}
	if (type == Type::I32) {
		return VM::create_i32(c.F, value);
	}
	if (type == Type::I64) {
		return VM::create_i64(c.F, value);
	}
	assert(0);
	return nullptr;
}

}


// TODO : convertion from text done here (useful for i64)
