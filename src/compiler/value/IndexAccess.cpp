#include "IndexAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSMap.hpp"
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
	if (key2) {
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

bool IndexAccess::isLeftValue() const
{
	return key2 == nullptr;
}

void IndexAccess::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	container->analyse(analyser, Type::UNKNOWN);
	constant = container->constant;

	if (container->type.raw_type != RawType::VEC && container->type.raw_type != RawType::MAP) {
		stringstream oss;
		container->print(oss);
		analyser->add_error({ SemanticException::CANNOT_INDEX_THIS, container->line(), oss.str() });
	}

	// VEC
	if (container->type.raw_type == RawType::VEC) {
		key->analyse(analyser, Type::I32);
		constant = constant && key->constant;
		type = container->type.getElementType(0);

		if (key2) {
			key2->analyse(analyser, Type::I32);
			constant = constant && key2->constant;
			type = container->type;
		}
	}

	// MAP
	if (container->type.raw_type == RawType::MAP) {
		key->analyse(analyser, container->type.getElementType(0));
		constant = constant && key->constant;
		type = container->type.getElementType(1);

		if (key2) {
			key2->analyse(analyser, container->type.getElementType(0));
			constant = constant && key2->constant;
			type = Type(RawType::VEC, { container->type.getElementType(1) });
		}
	}
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

LSValue* IA_vec_lsptr(LSVec<LSValue*>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) return nullptr;
	if (i >= vec->size()) {
		if (vec->refs == 0) delete vec;
		return nullptr;
	}
	LSValue* r = (*vec)[i];
	if (vec->refs == 0) {
		(*vec)[i] = nullptr;
		delete vec;
	}
	return r;
}
int32_t IA_vec_32(LSVec<int32_t>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) return 0;
	int32_t r = (i < vec->size()) ? (*vec)[i] : 0;
	if (vec->refs == 0) delete vec;
	return r;
}
double IA_vec_64(LSVec<double>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) return 0.0;
	double r = (i < vec->size()) ? (*vec)[i] : 0.0;
	if (vec->refs == 0) delete vec;
	return r;
}

/*
LSValue* IA_map_lsptr_lsptr(LSMap<LSValue*,LSValue*>* map, LSValue* k) {
	VM::operations += log(map->size());
	auto i = map->find(k);
	if (vec->refs == 0) {
		(*vec)[i] = nullptr;
		delete vec;
	}
	return r;
}
int32_t IA_map_lsptr_32(LSVec<int32_t>* vec, int i) {
	VM::operations += 2;
	int32_t r = (*vec)[i];
	if (vec->refs == 0) delete vec;
	return r;
}
int64_t IA_map_lsptr_64(LSVec<int64_t>* vec, int i) {
	VM::operations += 2;
	int64_t r = (*vec)[i];
	if (vec->refs == 0) delete vec;
	return r;
}
*/

jit_value_t IndexAccess::compile(Compiler& c) const {
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	if (!key2) {
		jit_type_t args_types[2] = { LS_POINTER, key->type.raw_type.jit_type() };
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, container->type.getElementType(0).raw_type.jit_type(), args_types, 2, 0);
		jit_value_t args[] = { a, k };

		// VEC
		if (container->type.raw_type == RawType::VEC) {
			if (container->type.getElementType(0).raw_type.nature() == Nature::LSVALUE) return jit_insn_call_native(c.F, "access", (void*) IA_vec_lsptr, sig, args, 2, JIT_CALL_NOTHROW);
			if (container->type.getElementType(0).raw_type.bytes() == sizeof (int32_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_32, sig, args, 2, JIT_CALL_NOTHROW);
			if (container->type.getElementType(0).raw_type.bytes() == sizeof (int64_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_64, sig, args, 2, JIT_CALL_NOTHROW);
			// TODO tuple
			assert(0);
		}

		/*
		// MAP
		if (container->type.raw_type == RawType::MAP) {
			if (container->type.getElementType(0).raw_type.nature() == Nature::LSVALUE) {
				if (container->type.getElementType(1).raw_type.nature() == Nature::LSVALUE) return jit_insn_call_native(c.F, "access", (void*) IA_map_lsptr, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(1).raw_type.bytes() == sizeof (int32_t)) return jit_insn_call_native(c.F, "access", (void*) IA_map_32, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(1).raw_type.bytes() == sizeof (int64_t)) return jit_insn_call_native(c.F, "access", (void*) IA_map_64, sig, args, 2, JIT_CALL_NOTHROW);
			}
			if (container->type.getElementType(0).raw_type.bytes() == sizeof (int32_t)) {
				if (container->type.getElementType(0).raw_type.nature() == Nature::LSVALUE) return jit_insn_call_native(c.F, "access", (void*) IA_vec_lsptr, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(0).raw_type.bytes() == sizeof (int32_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_32, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(0).raw_type.bytes() == sizeof (int64_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_64, sig, args, 2, JIT_CALL_NOTHROW);
			}
			if (container->type.getElementType(0).raw_type.bytes() == sizeof (int64_t)) {
				if (container->type.getElementType(0).raw_type.nature() == Nature::LSVALUE) return jit_insn_call_native(c.F, "access", (void*) IA_vec_lsptr, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(0).raw_type.bytes() == sizeof (int32_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_32, sig, args, 2, JIT_CALL_NOTHROW);
				if (container->type.getElementType(0).raw_type.bytes() == sizeof (int64_t)) return jit_insn_call_native(c.F, "access", (void*) IA_vec_64, sig, args, 2, JIT_CALL_NOTHROW);
			}
			// TODO tuple
			assert(0);
		}
		*/

	} else {
		jit_value_t k2 = key2->compile(c);
	}

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

			if (type.raw_type.nature() == Nature::LSVALUE) {
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

//			if (array_element_type.raw_type.nature() == Nature::VALUE and type.raw_type.nature() == Nature::POINTER) {
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

LSValue** IA_l_vec_lsptr(LSVec<LSValue*>* vec, int i) {
	VM::operations += 2;
	if (vec->refs == 0) {
		delete vec;
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}
int32_t* IA_l_vec_32(LSVec<int32_t>* vec, int i) {
	VM::operations += 2;
	if (vec->refs == 0) {
		delete vec;
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}
int64_t* IA_l_vec_64(LSVec<int64_t>* vec, int i) {
	VM::operations += 2;
	if (vec->refs == 0) {
		delete vec;
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}

jit_value_t IndexAccess::compile_l(Compiler& c) const
{
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	jit_type_t args_types[2] = { container->type.raw_type.jit_type(), key->type.raw_type.jit_type() };
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);
	jit_value_t args[] = { a, k };

	// VEC
	if (container->type.raw_type == RawType::VEC) {
		if (container->type.getElementType(0).raw_type.nature() == Nature::LSVALUE) return jit_insn_call_native(c.F, "access", (void*) IA_l_vec_lsptr, sig, args, 2, JIT_CALL_NOTHROW);
		if (container->type.getElementType(0).raw_type.bytes() == sizeof (int32_t)) return jit_insn_call_native(c.F, "access", (void*) IA_l_vec_32, sig, args, 2, JIT_CALL_NOTHROW);
		if (container->type.getElementType(0).raw_type.bytes() == sizeof (int64_t)) return jit_insn_call_native(c.F, "access", (void*) IA_l_vec_64, sig, args, 2, JIT_CALL_NOTHROW);
		// TODO tuple
		assert(0);
	}


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
