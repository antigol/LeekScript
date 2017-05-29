#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <string>
#include <jit/jit.h>
#include <gmp.h>
#include <gmpxx.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "../compiler/lexical/LexicalError.hpp"
#include "../compiler/syntaxic/SyntaxicalError.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "../compiler/Compiler.hpp"
#include "Exception.hpp"

#define OPERATION_LIMIT 10000000

#define LS_VOID jit_type_void
#define LS_INTEGER jit_type_int
#define LS_LONG jit_type_long
#define LS_REAL jit_type_float64
#define LS_BOOLEAN jit_type_sys_bool
#define LS_POINTER jit_type_void_ptr
#define LS_STRING LS_POINTER
#define LS_NUMBER LS_POINTER

#define LS_CREATE_INTEGER(F, X) jit_value_create_nint_constant((F), LS_INTEGER, (X))
#define LS_CREATE_BOOLEAN(F, X) jit_value_create_nint_constant((F), LS_INTEGER, (X))
#define LS_CREATE_LONG(F, X) jit_value_create_long_constant((F), LS_LONG, (X))
#define LS_CREATE_REAL(F, X) jit_value_create_float64_constant((F), LS_REAL, (X))

struct jit_stack_trace {
	unsigned int size;
	void* items[1];
};

namespace ls {

class Type;
class Module;
class Program;
class LSValue;
class SemanticVar;
class LSNull;
class LSBoolean;
template <class R> class LSFunction;

class VM {
public:

	static const unsigned long int DEFAULT_OPERATION_LIMIT;
	static jit_type_t mpz_type;
	static VM* current_vm;

	struct Result {
		bool compilation_success = false;
		bool execution_success = false;
		std::vector<LexicalError> lexical_errors;
		std::vector<SyntaxicalError> syntaxical_errors;
		std::vector<SemanticError> semantical_errors;
		vm::ExceptionObj* exception = nullptr;
		std::string program = "";
		std::string value = "";
		std::string context = "";
		long compilation_time = 0;
		long compilation_time_ms = 0;
		long execution_time = 0;
		long execution_time_ms = 0;
		long operations = 0;
		int objects_created = 0;
		int objects_deleted = 0;
		int mpz_objects_created = 0;
		int mpz_objects_deleted = 0;
	};

	std::vector<Module*> modules;
	std::map<std::string, LSValue*> system_vars;
	std::map<std::string, std::shared_ptr<SemanticVar>> internal_vars;
	std::map<std::string, jit_value_t> internals;
	Compiler compiler;
	LSNull* null_value;
	LSBoolean* true_value;
	LSBoolean* false_value;
	unsigned int operations = 0;
	bool enable_operations = true;
	unsigned long int operation_limit;
	std::ostream* output = &std::cout;
	long mpz_created = 0;
	long mpz_deleted = 0;
	vm::ExceptionObj* last_exception = nullptr;
	jit_stack_trace_t stack_trace;
	jit_context_t jit_context;
	std::string file_name;

	VM(bool v1 = false);
	virtual ~VM();

	static VM* current();

	/** Main execution function **/
	Result execute(const std::string code, std::string ctx, std::string file_name, bool debug = false, bool ops = true);

	/** Add a module **/
	void add_module(Module* m);
	void add_internal_var(std::string name, Type type);
	static jit_type_t get_jit_type(const Type& type);

	/** Add a constant **/
	void add_constant(std::string name, Type type, LSValue* value);

	/** Value creation **/
	static jit_value_t create_array(jit_function_t F, const Type& element_type,
		int cap = 0);

	/** Conversions **/
	static jit_value_t value_to_pointer(jit_function_t, jit_value_t, Type);
	static jit_value_t pointer_to_value(jit_function_t, jit_value_t, Type);
	static jit_value_t int_to_real(jit_function_t F, jit_value_t v);

	/** Ref counting and memory management **/
	static jit_value_t get_refs(jit_function_t F, jit_value_t obj);
	static void inc_refs_if_not_temp(jit_function_t F, jit_value_t obj);
	static void dec_refs(jit_function_t F, jit_value_t obj);
	static void inc_mpz_counter(jit_function_t F);

	/** Utilities **/
	static void print_mpz_int(jit_function_t F, jit_value_t val);
	void store_exception(jit_function_t F, jit_value_t ex);

	static unsigned int get_offset(jit_context_t context, void* pc) {
		auto trace = (jit_stack_trace_t) jit_malloc(sizeof(struct jit_stack_trace));
		trace->size = 1;
		trace->items[0] = pc;
		auto line = jit_stack_trace_get_offset(context, trace, 0);
		jit_free(trace);
		return line;
	}

	template <unsigned int level>
	static void* get_exception_object(int obj) {
		auto ex = new vm::ExceptionObj((vm::Exception) obj);
		auto context = VM::current()->jit_context;
		auto frame = __builtin_frame_address(level);
		size_t N = 16;
		void* array[N];
		size_t size = backtrace(array, N);
		auto pc = array[size - 1]; // take last C++ stacktrace pc as first jit pc
		while (true) {
			auto line = get_offset(context, pc);
			if (line == JIT_NO_OFFSET) break;
			vm::exception_frame frame_object;
			frame_object.pc = pc;
			frame_object.frame = frame;
			frame_object.line = line;
			auto fun = jit_function_from_pc(context, pc, nullptr);
			auto name = fun ? (std::string*) jit_function_get_meta(fun, 12) : nullptr;
			auto file = fun ? (std::string*) jit_function_get_meta(fun, 13) : nullptr;
			frame_object.file = file == nullptr ? "?" : *file;
			frame_object.function = name == nullptr ? "?" : *name;
			ex->frames.emplace_back(frame_object);
			frame = jit_get_next_frame_address(frame);
			pc = jit_get_return_address(frame);
		}
		return ex;
	}
};

}

#endif
