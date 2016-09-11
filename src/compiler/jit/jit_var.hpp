#ifndef JIT_VAR_HPP
#define JIT_VAR_HPP

#include "jit_general.hpp"

namespace ls {

class jit_var
{
public:
	static jit_value_t convert(jit_function_t F, jit_value_t v, const Type& type);
	static jit_value_t create_bool(jit_function_t F, bool value);
	static jit_value_t create_real(jit_function_t F, double value);

	static void print(jit_function_t F, jit_value_t v);
};
}
#endif // JIT_VAR_HPP
