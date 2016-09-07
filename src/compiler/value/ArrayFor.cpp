#include "ArrayFor.hpp"

using namespace std;

namespace ls {

ArrayFor::~ArrayFor() {
	delete forr;
}

void ArrayFor::print(ostream& os, int indent, bool debug) const {
	os << "[";
	forr->print(os, indent, debug);
	os << "]";

	if (debug) {
		os << " " << type;
	}
}

unsigned ArrayFor::line() const {
	return 0;
}

void ArrayFor::analyse_help(SemanticAnalyser* analyser)
{
	forr->type = Type::VEC;
	forr->analyse(analyser);
	type = forr->type;
}

void ArrayFor::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	forr->reanalyse(analyser, req_type);
	type = forr->type;
}

void ArrayFor::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	forr->finalize(analyser, req_type);
	type = forr->type;
	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}


}
