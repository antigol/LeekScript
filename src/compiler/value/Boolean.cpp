#include "Boolean.hpp"
#include "../../vm/VM.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include <cassert>
#include "../jit/jit_general.hpp"
#include "../jit/jit_var.hpp"

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

// DONE 2
void Boolean::analyse_help(SemanticAnalyser*)
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
		return jit_var::create_bool(c.F, value);
	}
	if (type == Type::I32) {
		return jit_general::constant_i32(c.F, value);
	}
	if (type == Type::I64) {
		return jit_general::constant_i64(c.F, value);
	}
	if (type == Type::BOOLEAN) {
		return jit_general::constant_bool(c.F, value);
	}
	assert(0);
	return nullptr;
}

}
