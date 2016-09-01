#include "IndexAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

IndexAccess::IndexAccess() {
	container = nullptr;
	key = nullptr;
	key2 = nullptr;
}

IndexAccess::~IndexAccess() {
	delete container;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void IndexAccess::print(std::ostream& os, int indent, bool debug) const {
	container->print(os, indent, debug);
	os << "[";
	key->print(os, indent, debug);
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

unsigned IndexAccess::line() const {
	return 0;
}

void IndexAccess::analyse(SemanticAnalyser* analyser, const Type&) {
/*
	type = Type::POINTER;
	container->analyse(analyser, Type::UNKNOWN);
	Type key_type = Type::UNKNOWN;

	if (container->type.raw_type == RawType::ARRAY) {
		key_type = Type::INTEGER;
		type = container->type.getElementType();

	} else 	if (container->type.raw_type == RawType::MAP) {
		key_type = container->type.getElementType(0);
		type = container->type.getElementType(1);

	} else {

		analyser->add_error({SemanticException::Type::CANNOT_INDEX_THIS, 0});
	}

	key->analyse(analyser, key_type);
	if (key->type != key_type) {
		analyser->add_error({SemanticException::Type::INDEX_TYPE, 0, "<key 1>"});
	}

	if (key2) {
		key2->analyse(analyser, key_type);
		if (key2->type != key_type) {
			analyser->add_error({SemanticException::Type::INDEX_TYPE, 0, "<key 2>"});
		}
	}

	constant = container->constant && key->constant && (!key2 || key2->constant);
	*/
}

/*
LSValue* access_temp(LSVec<LSValue*>* array, LSValue* key) {
	return array->at(key);
}

int access_temp_value(LSVec<int>* array, int key) {
	return array->atv(key);
}

LSValue** access_l(LSVec<LSValue*>* array, LSValue* key) {
	return array->atL(key);
}

int* access_l_value(LSVec<int>* array, int key) {
	return array->atLv(key);
}

LSValue* range(LSValue* array, int start, int end) {
	LSValue* r = array->range(start, end);
	LSValue::delete_temporary(array);
	return r;
}

int interval_access(const LSInterval* interval, int pos) {
	return interval->atv(pos);
}*/

jit_value_t IndexAccess::compile(Compiler& c) const {

	/*
	jit_value_t a = container->compile(c);

	if (key2 == nullptr) {

		if (container->type == Type::INTERVAL) {

			jit_type_t args_types[2] = {LS_POINTER, LS_I32};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I32, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", (void*) interval_access, sig, args, 2, JIT_CALL_NOTHROW);

			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

			if (type.nature == Nature::LSVALUE) {
				return VM::value_to_lsvalue(c.F, res, type);
			}
			return res;

		} else {

			jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

			jit_value_t k = key->compile(c);

			void* func = type == Type::INTEGER ? (void*) access_temp_value : (void*) access_temp;

			jit_value_t args[] = {a, k};
			jit_value_t res = jit_insn_call_native(c.F, "access", func, sig, args, 2, JIT_CALL_NOTHROW);

			if (key->type.must_manage_memory()) {
				VM::delete_temporary(c.F, k);
			}
			VM::delete_temporary(c.F, a);

			// Array access : 2 operations
			VM::inc_ops(c.F, 2);

//			if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
//				return VM::value_to_pointer(c.F, res, type);
//			}
			return res;
		}

	} else {

		jit_type_t args_types[3] = {LS_POINTER, LS_I32, LS_I32};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 3, 0);

		jit_value_t start = key->compile(c);
		jit_value_t end = key2->compile(c);
		jit_value_t args[] = {a, start, end};

		jit_value_t result = jit_insn_call_native(c.F, "range", (void*) range, sig, args, 3, JIT_CALL_NOTHROW);

		return result;
	}
	*/
}

jit_value_t IndexAccess::compile_l(Compiler& c) const {
/*
	jit_value_t a = container->compile(c);

	jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

	jit_value_t k = key->compile(c);

	jit_value_t args[] = {a, k};
	return jit_insn_call_native(c.F, "access_l", (void*) access_l_value, sig, args, 2, JIT_CALL_NOTHROW);
	*/
}

}
