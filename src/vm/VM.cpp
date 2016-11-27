#include <sstream>
#include <chrono>

#include "VM.hpp"

#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "value/LSNumber.hpp"
#include "value/LSArray.hpp"
#include "Program.hpp"
#include "value/LSObject.hpp"

using namespace std;

namespace ls {

VM::VM() {}

VM::~VM() {}

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 2000000000;
unsigned int VM::operations = 0;
const bool VM::enable_operations = true;
unsigned long int VM::operation_limit = VM::DEFAULT_OPERATION_LIMIT;

jit_type_t VM::gmp_int_type;
long VM::gmp_values_created = 0;
long VM::gmp_values_deleted = 0;

VM::Exception VM::last_exception = VM::Exception::NO_EXCEPTION;
jit_stack_trace_t VM::stack_trace;

map<string, jit_value_t> internals;

void VM::add_module(Module* m) {
	modules.push_back(m);
}

#define GREY "\033[0;90m"
#define GREEN "\033[0;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define YELLOW "\033[1;33m"
#define END_COLOR "\033[0m"

VM::Result VM::execute(const std::string code, std::string ctx) {

	jit_type_t types[3] = {jit_type_int, jit_type_int, jit_type_void_ptr};
	VM::gmp_int_type = jit_type_create_struct(types, 3, 0);

	// Reset
	LSValue::obj_count = 0;
	LSValue::obj_deleted = 0;
	VM::gmp_values_created = 0;
	VM::gmp_values_deleted = 0;
	VM::operations = 0;
	VM::last_exception = VM::Exception::NO_EXCEPTION;
	#if DEBUG_LEAKS_DETAILS
		LSValue::objs().clear();
	#endif

	Program* program = new Program(code);

	// Compile
	auto compilation_start = chrono::high_resolution_clock::now();
	VM::Result result = program->compile(*this, ctx);
	auto compilation_end = chrono::high_resolution_clock::now();

	#if DEBUG_PRGM_TYPES
		std::cout << "main() " << result.program << std::endl;
	#endif

	// Execute
	std::string value;
	if (result.compilation_success) {

		auto exe_start = chrono::high_resolution_clock::now();
		try {
			value = program->execute();
			result.execution_success = true;
		} catch (const VM::Exception& ex) {
			result.exception = ex;
		}
		auto exe_end = chrono::high_resolution_clock::now();

		result.execution_time = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();
		result.execution_time_ms = (((double) result.execution_time / 1000) / 1000);

		result.value = value;
	}

	// Set results
	result.context = ctx;
	result.compilation_time = chrono::duration_cast<chrono::nanoseconds>(compilation_end - compilation_start).count();
	result.compilation_time_ms = (((double) result.compilation_time / 1000) / 1000);
	result.operations = VM::operations;

	// Cleaning
	delete program;

	result.objects_created = LSValue::obj_count;
	result.objects_deleted = LSValue::obj_deleted;
	result.gmp_objects_created = VM::gmp_values_created;
	result.gmp_objects_deleted = VM::gmp_values_deleted;

	if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
		cout << RED << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << END_COLOR << endl;
		#if DEBUG_LEAKS_DETAILS
			for (auto o : LSValue::objs()) {
				std::cout << o.second << " (" << o.second->refs << " refs)" << endl;
			}
		#endif
	}
	if (VM::gmp_values_deleted != VM::gmp_values_created) {
		cout << RED << "/!\\ " << VM::gmp_values_deleted << " / " << VM::gmp_values_created << " (" << (VM::gmp_values_created - VM::gmp_values_deleted) << " gmp leaked)" << END_COLOR << endl;
	}

	return result;
}

jit_type_t VM::get_jit_type(const Type& type) {
	if (type.nature == Nature::VOID) {
		return LS_VOID;
	}
	if (type.nature == Nature::POINTER) {
		return LS_POINTER;
	}
	if (type.raw_type == RawType::UNSIGNED_LONG) {
		return jit_type_ulong;
	}
	if (type.raw_type == RawType::UNSIGNED_INTEGER) {
		return jit_type_uint;
	}
	if (type.raw_type == RawType::INTEGER) {
		return LS_INTEGER;
	}
	if (type.raw_type == RawType::GMP_INT) {
		return VM::gmp_int_type;
	}
	if (type.raw_type == RawType::BOOLEAN) {
		return LS_BOOLEAN;
	}
	if (type.raw_type == RawType::LONG or type.raw_type == RawType::FUNCTION) {
		return LS_LONG;
	}
	if (type.raw_type == RawType::REAL) {
		return LS_REAL;
	}
	return LS_INTEGER;
}

LSValue* create_null_object(int) {
	return LSNull::get();
}
LSValue* create_number_object_int(int n) {
	return LSNumber::get(n);
}
LSValue* create_number_object_long(long n) {
	return LSNumber::get(n);
}
LSValue* create_bool_object(bool n) {
	return LSBoolean::get(n);
}
LSValue* create_func_object(void* f) {
	return new LSFunction(f);
}
LSValue* create_float_object(double n) {
	return LSNumber::get(n);
}

void* get_conv_fun(Type type) {
	if (type.raw_type == RawType::NULLL) {
		return (void*) &create_null_object;
	}
	if (type.raw_type == RawType::INTEGER) {
		return (void*) &create_number_object_int;
	}
	if (type.raw_type == RawType::LONG) {
		return (void*) &create_number_object_long;
	}
	if (type.raw_type == RawType::REAL) {
		return (void*) &create_float_object;
	}
	if (type.raw_type == RawType::BOOLEAN) {
		return (void*) &create_bool_object;
	}
	if (type.raw_type == RawType::FUNCTION) {
		return (void*) &create_func_object;
	}
	return (void*) &create_number_object_int;
}

jit_value_t VM::value_to_pointer(jit_function_t F, jit_value_t v, Type type) {

	void* fun = get_conv_fun(type);

	bool floatt = jit_type_get_kind(jit_value_get_type(v)) == JIT_TYPE_FLOAT64;
	if (floatt) {
		fun = (void*) &create_float_object;
	}

	jit_type_t args_types[1] = { get_jit_type(type) };

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);
	return jit_insn_call_native(F, "convert", (void*) fun, sig, &v, 1, JIT_CALL_NOTHROW);
}

int VM_boolean_to_value(LSBoolean* b) {
	return b->value;
}
int VM_integer_to_value(LSNumber* n) {
	return n->value;
}

jit_value_t VM::pointer_to_value(jit_function_t F, jit_value_t v, Type type) {

	if (type == Type::BOOLEAN) {
		jit_type_t args_types[1] = {LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 0);
		return jit_insn_call_native(F, "convert", (void*) VM_boolean_to_value, sig, &v, 1, JIT_CALL_NOTHROW);
	}
	if (type == Type::INTEGER) {
		jit_type_t args_types[1] = {LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 0);
		return jit_insn_call_native(F, "convert", (void*) VM_integer_to_value, sig, &v, 1, JIT_CALL_NOTHROW);
	}
	return LS_CREATE_INTEGER(F, 0);
}

jit_value_t VM::int_to_real(jit_function_t F, jit_value_t v) {
	jit_value_t real = jit_value_create(F, LS_REAL);
	jit_insn_store(F, real, v);
	return real;
}

/*
bool VM::get_number(jit_function_t F, jit_value_t val) {

	// x & (1 << 31) == 0

	jit_value_t is_int = jit_insn_eq(F,
		jit_insn_and(F, val, jit_value_create_nint_constant(F, jit_type_int, 2147483648)),
		jit_value_create_nint_constant(F, jit_type_int, 0)
	);

	jit_value_t res;

	jit_label_t label_else;
	jit_insn_branch_if_not(F, is_int, &label_else);

	return false;
}*/

int VM_get_refs(LSValue* val) {
	return val->refs;
}

jit_value_t VM::get_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "get_refs", (void*) VM_get_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs(LSValue* val) {
	val->refs++;
}

void VM::inc_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "inc_refs", (void*) VM_inc_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs_if_not_temp(LSValue* val) {
	if (val->refs != 0) {
		val->refs++;
	}
}

void VM::inc_refs_if_not_temp(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "inc_refs_not_temp", (void*) VM_inc_refs_if_not_temp, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_dec_refs(LSValue* val) {
	val->refs--;
}

void VM::dec_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "dec_refs", (void*) VM_dec_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::delete_ref(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "delete", (void*) &LSValue::delete_ref, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::delete_temporary(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "delete_temporary", (void*) &LSValue::delete_temporary, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::inc_ops(jit_function_t F, int amount) {
	jit_value_t amount_jit = LS_CREATE_INTEGER(F, amount);
	inc_ops_jit(F, amount_jit);
}

void VM::inc_ops_jit(jit_function_t F, jit_value_t amount) {
	// Operations enabled?
	if (not enable_operations) return;

	// Variable counter pointer
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, LS_POINTER, (long int) &VM::operations);

	// Increment counter
	jit_value_t jit_ops = jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);
	jit_insn_store_relative(F, jit_ops_ptr, 0, jit_insn_add(F, jit_ops, amount));

	// Compare to the limit
	jit_value_t compare = jit_insn_gt(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, VM::operation_limit));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, compare, &label_end);

	// If greater than the limit, throw exception
	jit_insn_throw(F, LS_CREATE_INTEGER(F, VM::Exception::OPERATION_LIMIT_EXCEEDED));

	// End
	jit_insn_label(F, &label_end);
}

void VM_print_gmp_int(__mpz_struct mpz) {
	std::cout << "_mp_alloc: " << mpz._mp_alloc << std::endl;
	std::cout << "_mp_size: " << mpz._mp_size << std::endl;
	std::cout << "_mp_d: " << mpz._mp_d << std::endl;
	char buff[1000];
	mpz_get_str(buff, 10, &mpz);
	cout << "VM::print_gmp_int : " << buff << endl;
}
void VM::print_gmp_int(jit_function_t F, jit_value_t val) {
	jit_type_t args[1] = {get_jit_type(Type::GMP_INT)};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "print_gmp_int", (void*) VM_print_gmp_int, sig, &val, 1, JIT_CALL_NOTHROW);
}

LSObject* VM_create_object() {
	return new LSObject();
}

jit_value_t VM::create_object(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void_ptr, {}, 0, 0);
	return jit_insn_call_native(F, "create_object", (void*) VM_create_object, sig, {}, 0, JIT_CALL_NOTHROW);
}

LSArray<LSValue*>* VM_create_array_ptr(int cap) {
	LSArray<LSValue*>* array = new LSArray<LSValue*>();
	array->reserve(cap);
	return array;
}

LSArray<int>* VM_create_array_int(int cap) {
	LSArray<int>* array = new LSArray<int>();
	array->reserve(cap);
	return array;
}

LSArray<double>* VM_create_array_float(int cap) {
	LSArray<double>* array = new LSArray<double>();
	array->reserve(cap);
	return array;
}

jit_value_t VM::create_array(jit_function_t F, const Type& element_type, int cap) {
	jit_type_t args[1] = {LS_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	jit_value_t s = LS_CREATE_INTEGER(F, cap);

	if (element_type == Type::INTEGER) {
		return jit_insn_call_native(F, "create_array", (void*) VM_create_array_int, sig, &s, 1, JIT_CALL_NOTHROW);
	}
	if (element_type == Type::REAL) {
		return jit_insn_call_native(F, "create_array", (void*) VM_create_array_float, sig, &s, 1, JIT_CALL_NOTHROW);
	}
	return jit_insn_call_native(F, "create_array", (void*) VM_create_array_ptr, sig, &s, 1, JIT_CALL_NOTHROW);
}

void VM_push_array_ptr(LSArray<LSValue*>* array, LSValue* value) {
	array->push_move(value);
}

void VM_push_array_int(LSArray<int>* array, int value) {
	array->push_back(value);
}

void VM_push_array_float(LSArray<double>* array, double value) {
	array->push_back(value);
}

void VM::push_move_array(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value) {
	/* Because of the move, there is no need to call delete_temporary on the pushed value.
	 * If value points to a temporary variable his ownership will be transfer to the array.
	 */
	jit_type_t args[2] = {LS_POINTER, get_jit_type(element_type)};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 2, 0);
	jit_value_t args_v[] = {array, value};

	if (element_type == Type::INTEGER) {
		jit_insn_call_native(F, "push_array", (void*) VM_push_array_int, sig, args_v, 2, JIT_CALL_NOTHROW);
	} else if (element_type == Type::REAL) {
		jit_insn_call_native(F, "push_array", (void*) VM_push_array_float, sig, args_v, 2, JIT_CALL_NOTHROW);
	} else {
		jit_insn_call_native(F, "push_array", (void*) VM_push_array_ptr, sig, args_v, 2, JIT_CALL_NOTHROW);
	}
}

jit_value_t VM::create_gmp_int(jit_function_t F, long value) {

	jit_value_t gmp_struct = jit_value_create(F, gmp_int_type);
	jit_value_set_addressable(gmp_struct);

	jit_value_t gmp_addr = jit_insn_address_of(F, gmp_struct);
	jit_value_t jit_value = LS_CREATE_LONG(F, value);
	VM::call(F, LS_VOID, {LS_POINTER, LS_LONG}, {gmp_addr, jit_value}, &mpz_init_set_ui);

	VM::inc_gmp_counter(F);

	return gmp_struct;
}

LSValue* VM_move(LSValue* val) {
	return val->move();
}

jit_value_t VM::move_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "move", (void*) VM_move, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

LSValue* VM_move_inc(LSValue* val) {
	return val->move_inc();
}

jit_value_t VM::move_inc_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "move_inc", (void*) VM_move_inc, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

LSValue* VM_clone(LSValue* val) {
	return val->clone();
}

jit_value_t VM::clone_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "clone", (void*) VM_clone, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

void VM::delete_gmp_int(jit_function_t F, jit_value_t gmp) {

	jit_value_t gmp_addr = jit_insn_address_of(F, gmp);
	VM::call(F, LS_VOID, {LS_POINTER}, {gmp_addr}, &mpz_clear);

	// Increment gmp values counter
	jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &VM::gmp_values_deleted);
	jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

jit_value_t VM::clone_gmp_int(jit_function_t F, jit_value_t gmp) {

	jit_value_t r = VM::create_gmp_int(F);
	jit_value_t r_addr = jit_insn_address_of(F, r);
	jit_value_t gmp_addr = jit_insn_address_of(F, gmp);
	VM::call(F, LS_VOID, {LS_POINTER, LS_POINTER}, {r_addr, gmp_addr}, &mpz_init_set);
	return r;
}

void VM::inc_gmp_counter(jit_function_t F) {

	jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &VM::gmp_values_created);
	jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

bool VM_is_true(LSValue* val) {
	return val->isTrue();
}

jit_value_t VM::is_true(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args, 1, 0);
	return jit_insn_call_native(F, "is_true", (void*) VM_is_true, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

void VM::store_exception(jit_function_t F, jit_value_t ex) {
	/*
	VM::call(F, LS_VOID, {}, {}, +[] () {
		std::cout << "Exception saved!" << std::endl;
	});
	*/
	jit_value_t vm_ex_ptr = jit_value_create_long_constant(F, LS_POINTER, (long int) &VM::last_exception);
	jit_insn_store_relative(F, vm_ex_ptr, 0, ex);
}

std::string VM::exception_message(VM::Exception expected) {
	switch (expected) {
	case Exception::DIVISION_BY_ZERO: return "division_by_zero";
	case Exception::OPERATION_LIMIT_EXCEEDED: return "too_much_operations";
	case Exception::NUMBER_OVERFLOW: return "number_overflow";
	case Exception::NO_EXCEPTION: return "no_exception";
	case Exception::OTHER: return "?";
	}
	return "?";
}

void VM::function_add_capture(jit_function_t F, jit_value_t fun, jit_value_t capture) {
	VM::call(F, LS_VOID, {LS_POINTER, LS_POINTER}, {fun, capture}, +[](LSFunction* fun, LSValue* cap) {
		fun->add_capture(cap);
	});
}

jit_value_t VM::function_get_capture(jit_function_t F, jit_value_t fun_ptr, int capture_index) {
	jit_value_t jit_index = LS_CREATE_INTEGER(F, capture_index);
	return VM::call(F, LS_POINTER, {LS_POINTER, LS_INTEGER}, {fun_ptr, jit_index}, +[](LSFunction* fun, int index) {
		LSValue* v = fun->get_capture(index);
//		v->refs++;
		return v;
	});
}

}
