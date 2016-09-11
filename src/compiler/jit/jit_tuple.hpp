#ifndef JIT_TUPLE_H
#define JIT_TUPLE_H

#include "jit_general.hpp"

namespace ls {

class jit_tuple
{
public:
	static jit_type_t jit_type(const Type& type);
	static jit_value_t eq(jit_function_t F, jit_value_t v1, jit_value_t v2, const Type& type);
	static jit_value_t lt(jit_function_t F, jit_value_t v1, jit_value_t v2, const Type& type);
	static void delete_ref(jit_function_t F, jit_value_t v, const Type& type);
	static void delete_temporary(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t move_inc(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t move(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t create_def(jit_function_t F, const Type& type);
	static void inc_refs(jit_function_t F, jit_value_t v, const Type& type);
	static void print(jit_function_t F, jit_value_t v, const Type& type);
};
}

#endif // JIT_TUPLE_H
