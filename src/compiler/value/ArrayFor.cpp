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

void ArrayFor::analyse(SemanticAnalyser* analyser, const Type&) {
	forr->analyse(analyser, Type::VEC);
	type = forr->type;
	assert(type.is_complete());
}

jit_value_t ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}


}
