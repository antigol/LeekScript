#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <jit/jit.h>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include "../vm/Type.hpp"
#include "../vm/Exception.hpp"
#include "../vm/LSValue.hpp"

namespace ls {

class Program;
class VM;

class Compiler {
public:

	struct value {
		jit_value_t v;
		Type t;
		bool operator == (const value& o) const {
			return v == o.v and t == o.t;
		}
		bool operator != (const value& o) const {
			return v != o.v or t != o.t;
		}
	};

	struct catcher {
		jit_label_t start;
		jit_label_t end;
		jit_label_t handler;
		jit_label_t next;
	};

	jit_function_t F = nullptr;
	std::stack<jit_function_t> functions;
	std::vector<int> functions_blocks; // how many blocks are open in the current loop

	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<jit_label_t*> loops_end_labels;
	std::vector<jit_label_t*> loops_cond_labels;
	std::vector<std::vector<value>> function_variables;
	std::vector<std::map<std::string, value>> variables;
	std::vector<std::vector<catcher>> catchers;

	Program* program;
	VM* vm;

	Compiler(VM* vm);
	virtual ~Compiler();

	// Value creation
	value clone(value) const;
	value new_null() const;
	value new_bool(bool b) const;
	value new_integer(int i) const;
	value new_long(long l) const;
	value new_pointer(const void* p) const;
	value new_object() const;
	value new_object_class(value clazz) const;
	value new_mpz(long value = 0) const;

	// Conversions
	value to_int(value) const;
	value to_long(value) const;

	// Operators wrapping
	value insn_not(value) const;
	value insn_and(value, value) const;
	value insn_or(value, value) const;
	value insn_add(value, value) const;
	value insn_sub(value, value) const;
	value insn_eq(value, value) const;
	value insn_ne(value, value) const;
	value insn_lt(value, value) const;
	value insn_le(value, value) const;
	value insn_gt(value, value) const;
	value insn_ge(value, value) const;
	value insn_mul(value, value) const;
	value insn_div(value, value) const;
	value insn_int_div(value, value) const;
	value insn_bit_and(value, value) const;
	value insn_bit_or(value, value) const;
	value insn_bit_xor(value, value) const;
	value insn_mod(value, value) const;
	value insn_pow(value, value) const;
	value insn_log10(value) const;

	// Value management
	value insn_to_pointer(value v) const;
	value insn_to_bool(value v) const;
	value insn_address_of(value v) const;
	value insn_load(value v, int pos = 0, Type t = Type::POINTER) const;
	void  insn_store(value, value) const;
	void  insn_store_relative(value, int, value) const;
	value insn_typeof(value v) const;
	value insn_class_of(value v) const;
	void  insn_delete(value v) const;
	void  insn_delete_temporary(value v) const;
	value insn_array_size(value v) const;
	value insn_get_capture(int index, Type type) const;
	void  insn_push_array(value array, value element) const;
	value insn_move_inc(value) const;
	value insn_clone_mpz(value mpz) const;
	void  insn_delete_mpz(value mpz) const;
	value insn_inc_refs(value v) const;
	value insn_dec_refs(value v, value previous = {nullptr, Type::NULLL}) const;
	value insn_move(value v) const;
	value insn_refs(value v) const;
	value insn_native(value v) const;

	// Iterators
	value iterator_begin(value v) const;
	value iterator_end(value v, value it) const;
	value iterator_get(value it, value previous) const;
	value iterator_key(value v, value it, value previous) const;
	void iterator_increment(value it) const;

	// Controls
	void insn_if(value v, std::function<void()> then) const;
	void insn_if_not(value v, std::function<void()> then) const;
	void insn_throw(value v) const;
	void insn_throw_object(vm::Exception type) const;

	// Call functions
	template <typename R, typename... A>
	value insn_call(Type return_type, std::vector<value> args, R(*func)(A...)) const {
		return insn_call(return_type, args, (void*) func);
	}
	value insn_call(Type return_type, std::vector<value> args, void* func) const;
	void function_add_capture(Compiler::value fun, Compiler::value capture);
	void log(const std::string&& str) const;

	// Blocks
	void enter_block();
	void leave_block();
	void delete_variables_block(int deepness); // delete all variables in the #deepness current blocks
	void enter_function(jit_function_t F);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables();

	// Variables
	void add_var(const std::string& name, jit_value_t value, const Type& type, bool ref);
	void add_function_var(jit_value_t value, const Type& type);
	value& get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
	std::map<std::string, value> get_vars();
	void update_var(std::string& name, jit_value_t value, const Type& type);

	// Loops
	void enter_loop(jit_label_t*, jit_label_t*);
	void leave_loop();
	jit_label_t* get_current_loop_end_label(int deepness) const;
	jit_label_t* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;

	/** Operations **/
	void inc_ops(int add);
	void inc_ops_jit(value add);
	void get_operations();

	/** Exceptions **/
	void add_catcher(jit_label_t start, jit_label_t end, jit_label_t handler);
	void insn_check_args(std::vector<value> args, std::vector<LSValueType> types) const;
};

}

#endif
