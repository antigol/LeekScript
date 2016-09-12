#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>

#define OPERATION_LIMIT 10000000

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
	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_ops(jit_function_t F, int add);
	static void get_operations(jit_function_t F);
	static void print_int(jit_function_t F, jit_value_t val);
//	static jit_value_t create_vec(jit_function_t F, const Type& element_type, int cap = 0);
//	static void push_move_inc_vec(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value);
	static jit_value_t clone_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t clone_temporary_obj(jit_function_t F, jit_value_t ptr);
};

}

#endif
