#ifndef JIT_GENERAL_HPP
#define JIT_GENERAL_HPP

#include <vector>
#include <jit/jit.h>
#include "../../vm/Type.hpp"

#include "jit_tuple.hpp"
#include "jit_vec.hpp"
#include "jit_set.hpp"
#include "jit_var.hpp"

#define LS_I32 jit_type_int       // 32-bit int
#define LS_I64 jit_type_long      // 64-bit int
#define LS_F32 jit_type_float32
#define LS_F64 jit_type_float64
#define LS_BOOLEAN LS_I32
#define LS_POINTER jit_type_void_ptr
#define LS_VOID jit_type_void

namespace ls {

class jit_general
{
public:
	static jit_value_t call_native(jit_function_t F, jit_type_t return_type, std::vector<jit_type_t> args_type, void* function, std::vector<jit_value_t> args);

	static jit_value_t eq(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);
	static jit_value_t ne(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);
	static jit_value_t lt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);
	static jit_value_t le(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);
	static jit_value_t gt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);
	static jit_value_t ge(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2);

	static jit_value_t convert(jit_function_t F, jit_value_t v, const Type& t_in, const Type& t_out);
	static jit_value_t is_true_delete_temporary(jit_function_t F, jit_value_t v, const Type& type);

	static void delete_ref(jit_function_t F, jit_value_t v, const Type& type);
	static void delete_temporary(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t move_inc(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t move(jit_function_t F, jit_value_t v, const Type& type);
	static void inc_refs(jit_function_t F, jit_value_t v, const Type& type);

	static jit_value_t constant_bool(jit_function_t F, bool value);
	static jit_value_t constant_i32(jit_function_t F, int32_t value);
	static jit_value_t constant_i64(jit_function_t F, int64_t value);
	static jit_value_t constant_f32(jit_function_t F, double value);
	static jit_value_t constant_f64(jit_function_t F, double value);
	static jit_value_t constant_ptr(jit_function_t F, void* value);
	static jit_value_t constant_default(jit_function_t F, const Type& type);

	static void print(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t string(jit_function_t F, jit_value_t v, const Type& type);

	static void* closure_lt(const Type& type);
};
}

#endif // JIT_GENERAL_HPP
