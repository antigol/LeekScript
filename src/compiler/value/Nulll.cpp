#include "../../compiler/value/Nulll.hpp"

#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll() {
	type = Type::NULLL;
	constant = true;
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

void Nulll::analyse(SemanticAnalyser*, const Type&) {
	// nothing to do, always a pointer
}

Compiler::value Nulll::compile(Compiler& c) const {
	return c.new_null();
}

}
