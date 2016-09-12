#ifndef JIT_VEC_H
#define JIT_VEC_H

#include <vector>
#include "jit_general.hpp"

namespace ls {

class jit_vec
{
public:
	static jit_type_t jit_type();
	static jit_value_t create(jit_function_t F);
	static void push_move_inc(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value);
	static jit_value_t index(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index);
	static jit_value_t index_delete_temporary(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index);
	static jit_value_t index_l(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index);
	static jit_value_t size(jit_function_t F, const Type& element_type, jit_value_t array);
	static void delete_ref(jit_function_t F, const Type& element_type, jit_value_t array);
	static void delete_temporary(jit_function_t F, const Type& element_type, jit_value_t array);

	static jit_value_t eq(jit_function_t F, const Type& element_type, jit_value_t array1, jit_value_t array2);

	static jit_value_t move(jit_function_t F, const Type& element_type, jit_value_t array);
	static jit_value_t move_inc(jit_function_t F, const Type& element_type, jit_value_t array);
	static void inc_refs(jit_function_t F, jit_value_t array);

	static void print(jit_function_t F, const Type& element_type, jit_value_t array);
	static jit_value_t string(jit_function_t F, const Type& element_type, jit_value_t array);
};
}
#endif // JIT_VEC_H
