#include "../../compiler/value/Reference.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

Reference::Reference(Token *variable)
	: Value(), variable(variable), var(nullptr)
{
	type = Type::VALUE;
}

Reference::~Reference() {}

void Reference::print(ostream& os, int, bool debug) const {
	os << "@" << variable->content;
	if (debug) {
		os << " " << type;
	}
}

unsigned Reference::line() const {
	return 0;
}

void Reference::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(variable);
	type = var->type;

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}

//	cout << "ref " << variable->content << " : " << type << endl;
}

jit_value_t Reference::compile(Compiler& c) const {

//	cout << "Reference compile()" << endl;

	/*
	jit_value_t v;

	if (var->scope == VarScope::INTERNAL) {
		v = internals[variable->content];
	} else if (var->scope == VarScope::GLOBAL) {
		v = globals[variable->content];
	} else if (var->scope == VarScope::LOCAL) {
		v = locals[variable->content];
	} else { // var->scope == VarScope::PARAMETER
		v = jit_value_get_param(F, var->index);
	}

	if (var->type.nature == Nature::POINTER) {
		VM::inc_refs(F, v);
		return v;
	}
	if (req_type.nature == Nature::POINTER) {
		jit_value_t val = VM::value_to_pointer(F, v, req_type);
		VM::inc_refs(F, val);
		return val;
	}
	return v;
	*/

	if (type.nature == Nature::POINTER) {
		return LS_CREATE_POINTER(c.F, LSNull::get());
	} else {
		return LS_CREATE_INTEGER(c.F, 0);
	}
}

}
