#include "String.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

String::String(string& value, Token* token) {
	this->value = value;
	this->token = token;
}

String::~String() {
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

void String::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	type = Type::VAR;
	constant = true;

	if (req_type == Type::VOID) {
		type = Type::VOID;
		return;
	}

	if (!Type::get_intersection(type, req_type)) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

LSValue* String_create(string* s) {
	return new LSVar(*s);
}

jit_value_t String::compile(Compiler& c) const
{
	if (type == Type::VOID) return nullptr;
	jit_value_t base = VM::create_ptr(c.F, (void*) &value);

	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);

	return jit_insn_call_native(c.F, "create", (void*) String_create, sig, &base, 1, JIT_CALL_NOTHROW);
}

}
