#include "../value/String.hpp"
#include "../../vm/value/LSString.hpp"

using namespace std;

namespace ls {

String::String(std::shared_ptr<Token> token) : token(token) {
	type = Type::STRING;
	types = Type::STRING;
	type.temporary = true;
	constant = true;
	ls_string = new LSString(token->content);
}

String::~String() {
	delete ls_string;
}

void String::print(ostream& os, int, bool debug) const {
	os << "'" << token->content << "'";
	if (debug) {
		os << " " << type;
	}
}

Location String::location() const {
	return token->location;
}

void String::analyse(SemanticAnalyser*, const Type&) {
	// Nothing to do, always a pointer
}

Compiler::value String::compile(Compiler& c) const {
	auto base = c.new_pointer(ls_string);
	return c.insn_call(Type::STRING, {base}, (void*) +[](LSString* s) {
		return s->clone();
	});
}

Value* String::clone() const {
	return new String(token);
}

}
