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

void Nulll::preanalyse(SemanticAnalyser*)
{
	constant = true;
	type = Type::LSVALUE;
}

void Nulll::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void Nulll::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();
}

jit_value_t Nulll::compile(Compiler& c) const {
	return VM::create_null(c.F);
}

}
