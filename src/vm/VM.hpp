#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>
#include <gmp.h>
#include <gmpxx.h>

#include "../constants.h"
#include "Context.hpp"
#include "../compiler/lexical/LexicalError.hpp"
#include "../compiler/syntaxic/SyntaxicalError.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "../compiler/Compiler.hpp"

#define OPERATION_LIMIT 10000000

#define LS_VOID jit_type_void
#define LS_INTEGER jit_type_int
#define LS_LONG jit_type_long
#define LS_REAL jit_type_float64
#define LS_BOOLEAN LS_INTEGER
#define LS_POINTER jit_type_void_ptr
#define LS_STRING LS_POINTER
#define LS_NUMBER LS_POINTER

#define LS_CREATE_INTEGER(F, X) jit_value_create_nint_constant((F), LS_INTEGER, (X))
#define LS_CREATE_BOOLEAN(F, X) LS_CREATE_INTEGER(F, X)
#define LS_CREATE_LONG(F, X) jit_value_create_long_constant((F), LS_LONG, (X))
#define LS_CREATE_REAL(F, X) jit_value_create_float64_constant((F), LS_REAL, (X))
#define LS_CREATE_POINTER(F, X) jit_value_create_constant((F), new jit_constant_t { LS_POINTER, {(X)} })

namespace ls {

class Type;
class Module;
class Program;
class LSValue;
class LexicalError;
class Compiler;

class vm_operation_exception : public std::exception {
public:
	virtual const char* what() const throw (){
	   return "too much operations!";
	}
};

class VM {
public:

	enum Exception {
		DIVISION_BY_ZERO = -2,
		NO_EXCEPTION = 0,
		OPERATION_LIMIT_EXCEEDED = 1,
		NUMBER_OVERFLOW = 2,
		OTHER = 3
	};

	static const unsigned long int DEFAULT_OPERATION_LIMIT;
	static unsigned int operations;
	static const bool enable_operations;
	static unsigned long int operation_limit;

	static jit_type_t gmp_int_type;
	static long gmp_values_created;
	static long gmp_values_deleted;

	static Exception last_exception;
	static jit_stack_trace_t stack_trace;

	struct Result {
		bool compilation_success = false;
		bool execution_success = false;
		std::vector<LexicalError> lexical_errors;
		std::vector<SyntaxicalError*> syntaxical_errors;
		std::vector<SemanticError> semantical_errors;
		Exception exception = Exception::NO_EXCEPTION;
		std::string program;
		std::string value;
		std::string context;
		long compilation_time = 0;
		long compilation_time_ms = 0;
		long execution_time = 0;
		long execution_time_ms = 0;
		long operations = 0;
		int objects_created = 0;
		int objects_deleted = 0;
		int gmp_objects_created = 0;
		int gmp_objects_deleted = 0;
	};

	std::vector<Module*> modules;

	VM();
	virtual ~VM();

	/** Main execution function **/
	Result execute(const std::string code, std::string ctx);

	/** Add a module **/
	void add_module(Module* m);
	static jit_type_t get_jit_type(const Type& type);

	/** Value creation **/
	static jit_value_t get_null(jit_function_t F);
	static jit_value_t create_object(jit_function_t F);
	static jit_value_t create_array(jit_function_t F, const Type& element_type,
		int cap = 0);
	static void push_move_array(jit_function_t F, const Type& element_type,
		jit_value_t array, jit_value_t value);
	static jit_value_t create_gmp_int(jit_function_t F, long value = 0);

	/** Conversions **/
	static jit_value_t value_to_pointer(jit_function_t, jit_value_t, Type);
	static jit_value_t pointer_to_value(jit_function_t, jit_value_t, Type);
	static jit_value_t int_to_real(jit_function_t F, jit_value_t v);

	/** Ref counting and memory management **/
	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs_if_not_temp(jit_function_t F, jit_value_t obj);
	static void dec_refs(jit_function_t F, jit_value_t obj);
	static void delete_ref(jit_function_t F, jit_value_t obj);
	static void delete_temporary(jit_function_t F, jit_value_t obj);
	static jit_value_t move_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t move_inc_obj(jit_function_t F, jit_value_t ptr);
	static jit_value_t clone_obj(jit_function_t F, jit_value_t ptr);
	static void delete_gmp_int(jit_function_t F, jit_value_t gmp);

	/** Operations **/
	static void inc_ops(jit_function_t F, int add);
	static void inc_ops_jit(jit_function_t F, jit_value_t add);
	static void get_operations(jit_function_t F);

	/** Captures **/
	static void function_add_capture(jit_function_t F, jit_value_t, jit_value_t);
	static jit_value_t function_get_capture(jit_function_t F, jit_value_t fun_ptr, int capture_index);

	/** Utilities **/
	static void print_gmp_int(jit_function_t F, jit_value_t val);
	static jit_value_t is_true(jit_function_t F, jit_value_t ptr);
	static void store_exception(jit_function_t F, jit_value_t ex);
	static std::string exception_message(VM::Exception expected);

	template <typename R, typename... A>
	static jit_value_t call(jit_function_t& f, jit_type_t return_type, std::vector<jit_type_t> types, std::vector<jit_value_t> args, R(*func)(A...))
	{
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, return_type, types.data(), types.size(), 0);
		return jit_insn_call_native(f, "VM::call", (void*) func, sig, args.data(), types.size(), 0);
	}
	static jit_value_t call(jit_function_t& f, jit_type_t return_type, std::vector<jit_type_t> types, std::vector<jit_value_t> args, void* func)
	{
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, return_type, types.data(), types.size(), 0);
		return jit_insn_call_native(f, "VM::call", func, sig, args.data(), types.size(), 0);
	}
};

}

#endif
