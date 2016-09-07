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

void ArrayFor::preanalyse(SemanticAnalyser* analyser)
{
	forr->type = Type::VEC;
	forr->preanalyse(analyser);
	type = forr->type;
}

void ArrayFor::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	forr->will_require(analyser, req_type);
	type = forr->type;
}

void ArrayFor::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	forr->analyse(analyser, req_type);
	type = forr->type;
	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}


}
