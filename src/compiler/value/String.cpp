#include "../../compiler/value/String.hpp"

#include "../../vm/value/LSString.hpp"

using namespace std;

namespace ls {

String::String(string& value, Token* token) {
	this->value = value;
	this->token = token;
	type = Type::STRING;
	type.temporary = true;
	constant = true;
	ls_string = new LSString(value);
}

String::~String() {
	delete ls_string;
}

void String::print(ostream& os, int, bool debug) const {
	os << "'" << value << "'";
	if (debug) {
		os << " " << type;
	}
}

unsigned String::line() const {
	return token->line;
}

void String::analyse(SemanticAnalyser*, const Type&) {
	// Nothing to do, always a pointer
}

LSValue* String_create(LSString* s) {
	return s->clone();
}

Compiler::value String::compile(Compiler& c) const {

	jit_value_t base = LS_CREATE_POINTER(c.F, ls_string);

	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);

	return {jit_insn_call_native(c.F, "create", (void*) String_create, sig, &base, 1, JIT_CALL_NOTHROW), Type::STRING};
}

}
