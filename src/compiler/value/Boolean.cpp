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

void Boolean::analyse_help(SemanticAnalyser* analyser)
{
	constant = true;
	type = Type({ Type::BOOLEAN, Type::VAR, Type::I32 });
}

void Boolean::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void Boolean::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();
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
