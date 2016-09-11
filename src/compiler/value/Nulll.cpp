#include "Nulll.hpp"
#include "../jit/jit_general.hpp"

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

// DONE 2
void Nulll::analyse_help(SemanticAnalyser*)
{
	constant = true;
	type = Type::LSVALUE;
}

void Nulll::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void Nulll::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();
}

jit_value_t Nulll::compile(Compiler& c) const {
	return jit_general::constant_ptr(c.F, nullptr);
}

}
