#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>
#include <map>
#include <stack>

#include "../vm/Type.hpp"

namespace ls {

class Program;

class CompilerVar {
public:
	jit_value_t value;
	Type type;
	bool reference;
	CompilerVar() : value(jit_value_t{}), type(Type::UNKNOWN), reference(false) {}
	CompilerVar(jit_value_t value, const Type& type, bool reference) :
		value(value), type(type), reference(reference) {}
};

class Compiler {
public:

	struct value {
		jit_value_t v;
		Type t;
	};

	jit_function_t F = nullptr;
	std::stack<jit_function_t> functions;
	std::vector<int> functions_blocks; // how many blocks are open in the current loop

	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;
	std::vector<std::map<std::string, CompilerVar>> variables;

	Program* program;

	Compiler(Program* program);
	virtual ~Compiler();

	// Utils
	value signed_int(value) const;

	// Value creation
	value new_null() const;
	value new_bool(bool b) const;
	value new_integer(int i) const;
	value new_pointer(void* p) const;
	value new_mpz() const;

	// Operators wrapping
	value insn_not(value) const;
	value insn_and(value, value) const;
	value insn_or(value, value) const;
	value insn_add(value, value) const;
	value insn_eq(value, value) const;
	value insn_lt(value, value) const;
	value insn_gt(value, value) const;
	value insn_mul(value, value) const;

	// Value management
	value insn_to_pointer(value v) const;
	value insn_to_bool(value v) const;
	value insn_to_not_bool(value v) const;
	value insn_address_of(value v) const;
	value insn_typeof(value v) const;
	value insn_class_of(value v) const;
	void  insn_delete(value v) const;
	value insn_array_size(value v) const;
	value insn_get_capture(int index, Type type) const;

	// Call functions
	template <typename R, typename... A>
	value insn_call(Type return_type, std::vector<value> args, R(*func)(A...)) const {
		return insn_call(return_type, args, (void*) func);
	}
	value insn_call(Type return_type, std::vector<value> args, void* func) const;

	// Blocks
	void enter_block();
	void leave_block(jit_function_t F);
	void delete_variables_block(jit_function_t F, int deepness); // delete all variables in the #deepness current blocks
	void enter_function(jit_function_t F);
	void leave_function();
	int get_current_function_blocks() const;

	// Variables
	void add_var(const std::string& name, jit_value_t value, const Type& type, bool ref);
	CompilerVar& get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, CompilerVar> get_vars();

	// Loops
	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();
	jit_label_t* get_current_loop_end_label(int deepness) const;
	jit_label_t* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;
};

}

#endif
