#include "Set.hpp"
#include "../../vm/value/LSSet.hpp"

using namespace std;

namespace ls {

Set::Set() {
}

Set::~Set() {
	for (auto ex : expressions) delete ex;
}

void Set::print(ostream& os, int indent, bool debug) const {
	os << "<";
	for (size_t i = 0; i < expressions.size(); ++i) {
		if (i > 0) os << ", ";
		expressions[i]->print(os, indent + 1, debug);
	}
	os << ">";
	if (debug) os << " " << type;
}

unsigned Set::line() const {
	return 0;
}

void Set::analyse(SemanticAnalyser* analyser, const Type&) {

	Type element_type = Type::UNKNOWN;

	for (Value* ex : expressions) {
		ex->analyse(analyser, Type::UNKNOWN);
		element_type = Type::get_compatible_type(element_type, ex->type);
	}

	if (element_type.nature == Nature::VALUE) {
		if (element_type != Type::INTEGER && element_type != Type::REAL) {
			element_type = Type::POINTER;
		}
	} else if (element_type.nature == Nature::UNKNOWN) {
		element_type.nature = Nature::POINTER;
	}

	constant = true;
	for (Value* ex : expressions) {
		ex->analyse(analyser, element_type);
		constant = constant && ex->constant;
	}

	type = Type(RawType::SET, Nature::POINTER, element_type);
}

LSSet<LSValue*>* Set_create_ptr() { return new LSSet<LSValue*>(); }
LSSet<int>* Set_create_int()      { return new LSSet<int>();      }
LSSet<double>* Set_create_float() { return new LSSet<double>();   }

void Set_insert_ptr(LSSet<LSValue*>* set, LSValue* value) {
	auto it = set->lower_bound(value);
	if (it == set->end() || (**it != *value)) {
		set->insert(it, value->move_inc());
	}
	LSValue::delete_temporary(value);
}
void Set_insert_int(LSSet<int>* set, int value) {
	set->insert(value);
}
void Set_insert_float(LSSet<int>* set, double value) {
	set->insert(value);
}

jit_value_t Set::compile(Compiler& c) const {
	void* create = type.getElementType() == Type::INTEGER ? (void*) Set_create_int :
				   type.getElementType() == Type::REAL   ? (void*) Set_create_float :
															(void*) Set_create_ptr;
	void* insert = type.getElementType() == Type::INTEGER ? (void*) Set_insert_int :
				   type.getElementType() == Type::REAL   ? (void*) Set_insert_float :
															(void*) Set_insert_ptr;

	unsigned ops = 1;

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, {}, 0, 0);
	jit_value_t s = jit_insn_call_native(c.F, "create_set", (void*) create, sig, {}, 0, JIT_CALL_NOTHROW);

	double i = 0;
	for (Value* ex : expressions) {
		jit_value_t v = ex->compile(c);

		jit_type_t args[2] = {LS_POINTER, VM::get_jit_type(type.getElementType())};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 3, 0);
		jit_value_t args_v[] = {s, v};
		jit_insn_call_native(c.F, "insert", (void*) insert, sig, args_v, 2, JIT_CALL_NOTHROW);
		ops += std::log2(++i);
	}

	VM::inc_ops(c.F, ops);

	return s;
}


}
