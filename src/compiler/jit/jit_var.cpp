#include "jit_var.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;
using namespace ls;


LSValue* jit_var_convert_i32(int32_t n) {
	return new LSVar(n);
}
LSValue* jit_var_convert_i64(int64_t n) {
	return new LSVar(n);
}
LSValue* jit_var_convert_bool(int32_t n) {
	return new LSVar((bool) n);
}
LSValue* jit_var_convert_f32(float n) {
	return new LSVar(n);
}
LSValue* jit_var_convert_f64(double n) {
	return new LSVar(n);
}

jit_value_t jit_var::convert(jit_function_t F, jit_value_t v, const Type& type)
{
	if (type == Type::I32)     return jit_general::call_native(F, LS_POINTER, { LS_I32 }, (void*) jit_var_convert_i32, { v });
	if (type == Type::I64)     return jit_general::call_native(F, LS_POINTER, { LS_I64 }, (void*) jit_var_convert_i64, { v });
	if (type == Type::F32)     return jit_general::call_native(F, LS_POINTER, { LS_F32 }, (void*) jit_var_convert_f32, { v });
	if (type == Type::F64)     return jit_general::call_native(F, LS_POINTER, { LS_F64 }, (void*) jit_var_convert_f64, { v });
	if (type == Type::BOOLEAN) return jit_general::call_native(F, LS_POINTER, { LS_BOOLEAN }, (void*) jit_var_convert_bool, { v });
	assert(0);
	return nullptr;
}

LSValue* CP_create_bool(int value) {
	return new LSVar((bool) value);
}

jit_value_t jit_var::create_bool(jit_function_t F, bool value)
{
	return jit_general::call_native(F, LS_POINTER, { LS_BOOLEAN }, (void*) CP_create_bool, { jit_general::constant_bool(F, value) });
}

LSValue* CP_create_real(double value) {
	return new LSVar(value);
}

jit_value_t jit_var::create_real(jit_function_t F, double value)
{
	return jit_general::call_native(F, LS_POINTER, { LS_F64 }, (void*) CP_create_real, { jit_general::constant_f64(F, value) });
}

void jit_var_print(LSVar* value) {
	LSValue::print(cout, value);
}

void jit_var::print(jit_function_t F, jit_value_t v)
{
	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_var_print, { v });
}
