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

// DONE 2
void ArrayFor::analyse_help(SemanticAnalyser* analyser)
{
	forr->type = Type::VEC;
	forr->analyse(analyser);
	type = forr->type;
}

void ArrayFor::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	forr->reanalyse(analyser, type);
	type = forr->type;
}

void ArrayFor::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	forr->finalize(analyser, type);
	type = forr->type;
	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}


}
