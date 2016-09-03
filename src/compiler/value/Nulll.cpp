#include "Nulll.hpp"

using namespace std;

namespace ls {

Nulll::Nulll() {
}

Nulll::~Nulll() {}

void Nulll::print(ostream& os, int, bool debug) const {
	os << "null";
	if (debug) {
		os << " " << type;
	}
}

unsigned Nulll::line() const {
	return 0;
}

void Nulll::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	constant = true;

	if (req_type == Type::UNKNOWN) {
		type = Type::VAR;
	} else if (req_type.raw_type.nature() == Nature::LSVALUE) {
		type = req_type;
	} else {
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), "null" });
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

void Nulll::preanalyse(SemanticAnalyser*, const Type&)
{
	constant = true;
	type = Type::LSVALUE;
}

jit_value_t Nulll::compile(Compiler& c) const {
	return VM::create_null(c.F);
}

}
