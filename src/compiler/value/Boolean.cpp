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
	if (req_type != Type::UNKNOWN) {
		if (type.can_be_convert_in(req_type)) {
			type = req_type;
		} else {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}
}

jit_value_t Boolean::compile(Compiler& c) const {

	if (type == Type::VAR) {
		return VM::create_bool(c.F, value);
	}
	if (type == Type::I32) {
		return LS_CREATE_I32(c.F, value);
	}
	if (type == Type::BOOLEAN) {
		return LS_CREATE_BOOLEAN(c.F, value);
	}
	assert(0);
	return nullptr;
}

}
