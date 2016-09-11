#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>
#include <map>
#include <stack>

#include "../vm/Type.hpp"

namespace ls {

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

	jit_function_t F = nullptr;
	std::stack<jit_function_t> functions;
	std::vector<int> functions_blocks; // how many blocks are open in the current loop

	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;
	std::vector<std::map<std::string, CompilerVar>> variables;

	Compiler();
	virtual ~Compiler();

	void enter_block();
	void leave_block(jit_function_t F);
	void delete_variables_block(jit_function_t F, int deepness); // delete all variables in the #deepness current blocks
	void enter_function(jit_function_t F);
	void leave_function();
	int get_current_function_blocks() const;

	void add_var(const std::string& name, jit_value_t value, const Type& type, bool ref);
	CompilerVar& get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, CompilerVar> get_vars();

	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();
	jit_label_t* get_current_loop_end_label(int deepness) const;
	jit_label_t* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;
};

}

#endif
