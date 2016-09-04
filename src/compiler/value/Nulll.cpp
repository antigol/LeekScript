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
	type = Type::LSVALUE;
	if (!Type::get_intersection(type, req_type, &type)) {
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), "null" });
	}
	type.make_it_complete();
	assert(type.is_complete() || !analyser->errors.empty());
}

void Nulll::preanalyse(SemanticAnalyser*)
{
	constant = true;
	type = Type::LSVALUE;
}

jit_value_t Nulll::compile(Compiler& c) const {
	return VM::create_null(c.F);
}

}
