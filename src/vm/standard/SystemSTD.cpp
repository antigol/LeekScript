#include "SystemSTD.hpp"
#include "../../compiler/jit/jit_general.hpp"
#include "../LSValue.hpp"
#include <chrono>

using namespace std;
using namespace ls;


long System_get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long System_get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

jit_value_t System_operations(jit_function_t F) {
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, jit_type_void_ptr, (long int) &VM::operations);
	return jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);
}

jit_value_t System_time(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I64, {}, 0, 0);
	return jit_insn_call_native(F, "sec_time", (void*) System_get_sec_time, sig, {}, 0, JIT_CALL_NOTHROW);
}

jit_value_t System_millitime(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I64, {}, 0, 0);
	return jit_insn_call_native(F, "milli_time", (void*) System_get_milli_time, sig, {}, 0, JIT_CALL_NOTHROW);
}

jit_value_t System_microtime(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I64, {}, 0, 0);
	return jit_insn_call_native(F, "micro_time", (void*) System_get_micro_time, sig, {}, 0, JIT_CALL_NOTHROW);
}

jit_value_t System_nanotime(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I64, {}, 0, 0);
	return jit_insn_call_native(F, "nano_time", (void*) System_get_nano_time, sig, {}, 0, JIT_CALL_NOTHROW);
}

jit_value_t System_version(jit_function_t F) {
	return jit_value_create_nint_constant(F, jit_type_int, 1);
}

void System_print(LSValue* value) {
	std::cout << value;
	LSValue::delete_temporary(value);
	std::cout << std::endl;
}

void System_print_int(int v) {
	std::cout << v << std::endl;
}

void System_print_long(long v) {
	std::cout << v << std::endl;
}

void System_print_bool(bool v) {
	std::cout << std::boolalpha << v << std::endl;
}

void System_print_float(double v) {
	std::cout << v << std::endl;
}

void System_print_ln() { cout << endl; }

class Print : public Method {
public:
	Print() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::UNKNOWN);
		generic_type.set_return_type(Type::VOID);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		jit_general::print(c.F, args[0], type.arguments_types[0]);
		jit_general::call_native(c.F, LS_VOID, { }, (void*) System_print_ln, { });
		return nullptr;
	}
};

class String : public Method {
public:
	String() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::UNKNOWN);
		generic_type.set_return_type(Type::VAR);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		jit_value_t res = jit_general::string(c.F, args[0], type.arguments_types[0]);
		jit_general::delete_temporary(c.F, args[0], type.arguments_types[0]);
		return res;
	}
};

SystemSTD::SystemSTD() : Module("ls")
{
	methods["print"] = new Print();
	methods["string"] = new String();

/*
	static_field("version", Type::INTEGER, (void*) &System_version);

	static_field("operations", Type::INTEGER, (void*) &System_operations);

	static_field("time", Type::LONG, (void*) &System_time);
	static_field("milliTime", Type::LONG, (void*) &System_millitime);
	static_field("microTime", Type::LONG, (void*) &System_microtime);
	static_field("nanoTime", Type::LONG, (void*) &System_nanotime);

	static_method("print", {
		{Type::VOID, {Type::INTEGER}, (void*) &System_print_int},
		{Type::VOID, {Type::LONG}, (void*) &System_print_long},
		{Type::VOID, {Type::BOOLEAN}, (void*) &System_print_bool},
		{Type::VOID, {Type::FLOAT}, (void*) &System_print_float},
		{Type::VOID, {Type::POINTER}, (void*) &System_print}
	});
	*/
}



