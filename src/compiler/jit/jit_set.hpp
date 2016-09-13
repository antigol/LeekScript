#ifndef JIT_SET_HPP
#define JIT_SET_HPP

#include <set>
#include "jit_general.hpp"

namespace ls {

class jit_set
{
public:
	static jit_type_t jit_type();
	static jit_value_t create(jit_function_t F, const Type& value_type);
	static jit_value_t insert_move_inc(jit_function_t F, const Type& value_type, jit_value_t s, jit_value_t v);
	static std::pair<jit_value_t, jit_value_t> begin_end(jit_function_t F, jit_value_t s);
	static jit_value_t inc_iterator(jit_function_t F, jit_value_t i);
	static jit_value_t load_iterator(jit_function_t F, const Type& value_type, jit_value_t i);
	static jit_value_t size(jit_function_t F, jit_value_t s);
	static void delete_temporary(jit_function_t F, const Type& value_type, jit_value_t s);
	static void delete_ref(jit_function_t F, const Type& value_type, jit_value_t s);
	static jit_value_t move(jit_function_t F, const Type& value_type, jit_value_t s);
	static jit_value_t move_inc(jit_function_t F, const Type& value_type, jit_value_t s);
	static void inc_refs(jit_function_t F, jit_value_t s);
};
}

#endif // JIT_SET_HPP
