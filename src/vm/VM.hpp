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
	static void inc_ops(jit_function_t F, int add);
	static void get_operations(jit_function_t F);
	static void print_int(jit_function_t F, jit_value_t val);
};

}

#endif
