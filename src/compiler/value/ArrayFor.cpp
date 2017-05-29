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

Location ArrayFor::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void ArrayFor::analyse(SemanticAnalyser* analyser, const Type&) {
	forr->analyse(analyser, Type::PTR_ARRAY);
	type = forr->type;
}

Compiler::value ArrayFor::compile(Compiler& c) const {
	return forr->compile(c);
}

Value* ArrayFor::clone() const {
	auto af = new ArrayFor();
	af->forr = forr->clone();
	return af;
}

}
