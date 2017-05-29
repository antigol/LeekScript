#include "Nulll.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

Nulll::Nulll(std::shared_ptr<Token> token) : token(token) {
	type = Type::NULLL;
	constant = true;
}

void Nulll::print(ostream& os, int, bool debug) const {
	os << "null";
	if (debug) {
		os << " " << type;
	}
}

Location Nulll::location() const {
	return token->location;
}

void Nulll::analyse(SemanticAnalyser*, const Type&) {
	// nothing to do, always a pointer
}

Compiler::value Nulll::compile(Compiler& c) const {
	return c.new_null();
}

Value* Nulll::clone() const {
	return new Nulll(token);
}

}
