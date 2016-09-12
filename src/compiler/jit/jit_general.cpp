#include "jit_general.hpp"
#include "jit_vec.hpp"
#include "jit_tuple.hpp"
#include "jit_var.hpp"
#include "../../vm/LSValue.hpp"

using namespace std;
using namespace ls;

jit_value_t jit_general::call_native(jit_function_t F, jit_type_t return_type, std::vector<jit_type_t> args_type, void* function, std::vector<jit_value_t> args)
{
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, return_type, args_type.data(), args_type.size(), 0);
	jit_value_t res = jit_insn_call_native(F, "", (void*) function, sig, args.data(), args.size(), JIT_CALL_NOTHROW);
	jit_type_free(sig);
	return res;
}

bool CP_eq(LSValue* x, LSValue* y) {
	if (x == nullptr) return y == nullptr;
	if (y == nullptr) return false;
	return *x == *y;
}
bool CP_ne(LSValue* x, LSValue* y) {
	if (x == nullptr) return y != nullptr;
	if (y == nullptr) return true;
	return *x != *y;
}
bool CP_lt(LSValue* x, LSValue* y) {
	if (y == nullptr) return false;
	if (x == nullptr) return true;
	return *x < *y;
}
bool CP_le(LSValue* x, LSValue* y) {
	if (x == nullptr) return true;
	if (y == nullptr) return false;
	return *x <= *y;
}
bool CP_gt(LSValue* x, LSValue* y) {
	if (x == nullptr) return false;
	if (y == nullptr) return true;
	return *x > *y;
}
bool CP_ge(LSValue* x, LSValue* y) {
	if (y == nullptr) return true;
	if (x == nullptr) return false;
	return *x >= *y;
}

//  a?        a          a
// null < bool,number < text < vec < map < set < function < tuple

jit_value_t jit_general::eq(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_eq(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_eq(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_eq, { v1, v2 });
	}
	if (t1.raw_type == &RawType::TUPLE && t2 == t1) return jit_tuple::eq(F, v1, v2, t1);
	assert(0);
	return nullptr;
}

jit_value_t jit_general::ne(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_ne(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_ne(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_ne, { v1, v2 });
	}
	return jit_insn_to_not_bool(F, eq(F, v1, t1, v2, t2));
}

jit_value_t jit_general::lt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_lt(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_lt(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_lt, { v1, v2 });
	}
	if (t1.raw_type == &RawType::TUPLE && t2 == t1) return jit_tuple::lt(F, v1, v2, t1);
	assert(0);
	return nullptr;
}

jit_value_t jit_general::le(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_le(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_le(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_le, { v1, v2 });
	}
	return jit_insn_to_not_bool(F, lt(F, v2, t2, v1, t1));
}

jit_value_t jit_general::gt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_gt(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_gt(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_gt, { v1, v2 });
	}
	return lt(F, v2, t2, v1, t1);
}

jit_value_t jit_general::ge(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_ge(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_ge(F, v1, v2);
	if (t1 == Type::VAR && t2 == Type::VAR) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_ge, { v1, v2 });
	}
	return jit_insn_to_not_bool(F, lt(F, v1, t1, v2, t2));
}

jit_value_t jit_general::convert(jit_function_t F, jit_value_t v, const Type& t_in, const Type& t_out)
{
	if (t_in == t_out || v == nullptr) return v;
	if (t_out == Type::VAR) {
		return jit_var::convert(F, v, t_in);
	}
	if (t_in == Type::I32 && t_out == Type::F64) {
		return jit_insn_convert(F, v, t_out.jit_type(), 0);
	}
	if (t_in == Type::BOOLEAN && t_out == Type::I32) {
		return v;
	}

	// TODO add other possibilities here and in Type.cpp

	assert(0);
	return nullptr;
}

int32_t CP_is_true(LSValue* val) {
	if (val == nullptr) return 0;
	int32_t r = val->isTrue();
	if (val->refs == 0) delete val;
	return r;
}

jit_value_t jit_general::is_true_delete_temporary(jit_function_t F, jit_value_t v, const Type& type)
{
	if (type == Type::VAR) {
		return call_native(F, LS_I32, { LS_POINTER }, (void*) CP_is_true, { v });
	}
	if (Type::intersection(type, Type::VALUE_NUMBER)) return jit_insn_to_bool(F, v);
	assert(0);
}

void jit_general::delete_ref(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.is_pure());

	if (!type.must_manage_memory()) return;
	if (type == Type::VAR) {
		jit_general::call_native(F, jit_type_void, { LS_POINTER }, (void*) LSValue::delete_ref, { v });
		return;
	}
	if (type.raw_type == &RawType::TUPLE) {
		jit_tuple::delete_ref(F, v, type);
		return;
	}
	if (type.raw_type == &RawType::VEC) {
		jit_vec::delete_ref(F, type.elements_types[0], v);
		return;
	}
	assert(0);
}

void jit_general::delete_temporary(jit_function_t F, jit_value_t v, const Type& type)
{
	if (!type.must_manage_memory()) return;
	if (type == Type::VAR) {
		jit_general::call_native(F, jit_type_void, { LS_POINTER }, (void*) LSValue::delete_temporary, { v });
		return;
	}
	if (type.raw_type == &RawType::TUPLE) {
		jit_tuple::delete_temporary(F, v, type);
		return;
	}
	if (type.raw_type == &RawType::VEC) {
		jit_vec::delete_temporary(F, type.elements_types[0], v);
		return;
	}
	assert(0);
}

jit_value_t jit_general::move_inc(jit_function_t F, jit_value_t v, const Type& type)
{
	if (!type.must_manage_memory()) return v;
	if (type == Type::VAR) {
		return jit_general::call_native(F, LS_POINTER, { LS_POINTER }, (void*) LSValue::move_inc, { v });
	}
	if (type.raw_type == &RawType::TUPLE) return jit_tuple::move_inc(F, v, type);
	if (type.raw_type == &RawType::VEC) return jit_vec::move_inc(F, type.elements_types[0], v);
	assert(0);
}

jit_value_t jit_general::move(jit_function_t F, jit_value_t v, const Type& type)
{
	if (!type.must_manage_memory()) return v;
	if (type == Type::VAR) {
		return jit_general::call_native(F, LS_POINTER, { LS_POINTER }, (void*) LSValue::move, { v });
	}
	if (type.raw_type == &RawType::TUPLE) return jit_tuple::move(F, v, type);
	if (type.raw_type == &RawType::VEC) return jit_vec::move(F, type.elements_types[0], v);
	assert(0);
}

void jit_general::inc_refs(jit_function_t F, jit_value_t v, const Type& type)
{
	if (!type.must_manage_memory()) return;
	if (type == Type::VAR) {
		jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) LSValue::inc_refs, { v });
		return;
	}
	if (type.raw_type == &RawType::TUPLE) {
		jit_tuple::inc_refs(F, v, type);
		return;
	}
	if (type.raw_type == &RawType::VEC) {
		jit_vec::inc_refs(F, v);
		return;
	}
	assert(0);
}

jit_value_t jit_general::constant_bool(jit_function_t F, bool value)
{
	return constant_i32(F, value);
}

jit_value_t jit_general::constant_i32(jit_function_t F, int32_t value)
{
	return jit_value_create_nint_constant(F, LS_I32, value);
}

jit_value_t jit_general::constant_i64(jit_function_t F, int64_t value)
{
	return jit_value_create_long_constant(F, LS_I64, value);
}

jit_value_t jit_general::constant_f32(jit_function_t F, double value)
{
	return jit_value_create_float32_constant(F, LS_F32, value);
}

jit_value_t jit_general::constant_f64(jit_function_t F, double value)
{
	return jit_value_create_float64_constant(F, LS_F64, value);
}

jit_value_t jit_general::constant_ptr(jit_function_t F, void* value)
{
	jit_constant_t constant = { LS_POINTER, { value } };
	return jit_value_create_constant(F, &constant);
}

jit_value_t jit_general::constant_default(jit_function_t F, const Type& type)
{
	if (type == Type::VAR) return constant_ptr(F, nullptr);
	if (type == Type::VOID) return nullptr;
	if (type == Type::BOOLEAN) return constant_bool(F, false);
	if (type == Type::I32) return constant_i32(F, 0);
	if (type == Type::F64) return constant_f64(F, 0.0);
	if (type.raw_type == &RawType::FUNCTION) return constant_ptr(F, nullptr);
	if (type.raw_type == &RawType::TUPLE) return jit_tuple::create_def(F, type);
	if (type.raw_type == &RawType::VEC) return jit_vec::create(F);
	assert(0);
}

template <typename T>
void jit_general_print(T value) { cout << value; }
void jit_general_print_bool(int32_t value) { cout << (value ? "true" : "false"); }

void jit_general::print(jit_function_t F, jit_value_t v, const Type& type)
{
	if (type == Type::VAR) {
		jit_var::print(F, v);
		return;
	}
	if (type == Type::BOOLEAN) {
		call_native(F, LS_VOID, { LS_BOOLEAN }, (void*) jit_general_print_bool, { v });
		return;
	}
	if (type == Type::I32) {
		call_native(F, LS_VOID, { LS_I32 }, (void*) jit_general_print<int32_t>, { v });
		return;
	}
	if (type == Type::F64) {
		call_native(F, LS_VOID, { LS_F64 }, (void*) jit_general_print<double>, { v });
		return;
	}
	if (type.raw_type == &RawType::TUPLE) {
		jit_tuple::print(F, v, type);
		return;
	}
	if (type.raw_type == &RawType::VEC) {
		jit_vec::print(F, type.elements_types[0], v);
		return;
	}
	assert(0);
}

jit_value_t jit_general::string(jit_function_t F, jit_value_t v, const Type& type)
{
	if (type == Type::VAR) {
		return jit_var::string(F, v);
	}
	if (type == Type::BOOLEAN
			|| type == Type::I32
			|| type == Type::F64) {
		v = jit_var::convert(F, v, type);
		jit_value_t res = jit_var::string(F, v);
		jit_general::delete_temporary(F, v, Type::VAR);
		return res;
	}
	if (type.raw_type == &RawType::TUPLE) {
		return jit_tuple::string(F, v, type);
	}
	if (type.raw_type == &RawType::VEC) {
		return jit_vec::string(F, type.elements_types[0], v);
	}
	assert(0);
	return nullptr;
}
