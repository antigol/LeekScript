#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>

#define OPERATION_LIMIT 10000000

#define LS_I32 jit_type_int
#define LS_I64 jit_type_long
#define LS_F32 jit_type_float32
#define LS_F64 jit_type_float64
#define LS_BOOLEAN LS_I32
#define LS_POINTER jit_type_void_ptr
#define LS_VOID jit_type_void

namespace ls {

class Type;
class Module;
class Program;

enum class ExecMode {
	NORMAL, TOP_LEVEL, COMMAND_JSON, TEST, TEST_OPS, FILE_JSON
};

class vm_operation_exception : public std::exception {
public:
	virtual const char* what() const throw (){
	   return "too much operations!";
	}
};

class VM {
public:

	static unsigned int operations;
	static const bool enable_operations;
	static const unsigned int operation_limit;

	std::vector<Module*> modules;

	VM();
	virtual ~VM();

	/*
	 * Shorthand to execute a code
	 */
	std::string execute(const std::string code, std::string ctx, ExecMode mode);

	void add_module(Module* m);
	static jit_value_t value_to_lsvalue(jit_function_t, jit_value_t, Type);
	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs_if_not_temp(jit_function_t F, jit_value_t obj);
	static void dec_refs(jit_function_t F, jit_value_t obj);
	static void delete_ref(jit_function_t F, jit_value_t obj);
	static void delete_temporary(jit_function_t F, jit_value_t obj);
	static void inc_ops(jit_function_t F, int add);
	static void get_operations(jit_function_t F);
	static void print_int(jit_function_t F, jit_value_t val);
	static jit_value_t create_bool(jit_function_t F, bool value);
	static jit_value_t create_i32(jit_function_t F, int32_t value);
	static jit_value_t create_i64(jit_function_t F, int64_t value);
	static jit_value_t create_f32(jit_function_t F, double value);
	static jit_value_t create_f64(jit_function_t F, double value);
	static jit_value_t create_ptr(jit_function_t F, void* value);
	static jit_value_t create_null(jit_function_t F);
	static jit_value_t create_lsbool(jit_function_t F, bool value);
	static jit_value_t create_lsreal(jit_function_t F, double value);
	static jit_value_t create_default(jit_function_t F, const Type& type);
	static jit_value_t create_vec(jit_function_t F, const Type& element_type, int cap = 0);
	static void push_move_inc_vec(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value);
	static jit_value_t move_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t move_inc_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t clone_obj(jit_function_t F, jit_value_t ptr);
};

}

#endif
