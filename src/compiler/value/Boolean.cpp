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

void Boolean::analyse(SemanticAnalyser* analyser, const Type& req_type)
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

void Boolean::preanalyse(SemanticAnalyser*)
{
	constant = true;
	type = Type(RawType::UNKNOWN, { Type::BOOLEAN, Type::VAR, Type::I32, Type::I64 });
}

jit_value_t Boolean::compile(Compiler& c) const {

	if (type == Type::VAR) {
		return VM::create_lsbool(c.F, value);
	}
	if (type == Type::I32) {
		return VM::create_i32(c.F, value);
	}
	if (type == Type::I64) {
		return VM::create_i64(c.F, value);
	}
	if (type == Type::BOOLEAN) {
		return VM::create_bool(c.F, value);
	}
	assert(0);
	return nullptr;
}

}
