#include "Boolean.hpp"
#include "../../vm/VM.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include <cassert>

using namespace std;

namespace ls {

Boolean::Boolean(bool value) {
	this->value = value;
}

Boolean::~Boolean() {}

void Boolean::print(std::ostream& os, int, bool debug) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << type;
	}
}

unsigned Boolean::line() const {
	return 0;
}

void Boolean::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	constant = true;

	type = Type::BOOLEAN;
	if (req_type != Type::UNKNOWN && type.can_be_convert_in(req_type)) {
		type = req_type;
	}

	if (req_type != Type::UNKNOWN && type != req_type) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}
	assert(type.is_complete());
}

void Boolean::preanalyse(SemanticAnalyser* analyser, const Type& req_type)
{
	constant = true;
	if (req_type == Type::BOOLEAN || req_type == Type::VAR) {
		type = req_type;
	} else {
		type = Type(RawType::UNKNOWN, { Type::BOOLEAN, Type::VAR });
	}
}

jit_value_t Boolean::compile(Compiler& c) const {

	if (type == Type::VAR) {
		return VM::create_lsbool(c.F, value);
	}
	if (type == Type::I32) {
		return VM::create_i32(c.F, value);
	}
	if (type == Type::BOOLEAN) {
		return VM::create_bool(c.F, value);
	}
	assert(0);
	return nullptr;
}

}
