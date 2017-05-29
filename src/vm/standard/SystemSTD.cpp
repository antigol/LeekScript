#include "SystemSTD.hpp"

#include <chrono>
#include "../LSValue.hpp"
#include "../../constants.h"
#include "../VM.hpp"

namespace ls {

Compiler::value System_operations(Compiler& c);
Compiler::value System_version(Compiler& c);
Compiler::value System_time(Compiler& c);
Compiler::value System_millitime(Compiler& c);
Compiler::value System_microtime(Compiler& c);
Compiler::value System_nanotime(Compiler& c);
void System_print(LSValue* v);
void System_print_int(int v);
void System_print_mpz(__mpz_struct v);
void System_print_mpz_tmp(__mpz_struct v);
void System_print_long(long v);
void System_print_bool(bool v);
void System_print_float(double v);

SystemSTD::SystemSTD() : Module("System") {

	static_field("version", Type::INTEGER, (void*) &System_version);

	static_field("operations", Type::INTEGER, (void*) &System_operations);

	static_field("time", Type::LONG, (void*) &System_time);
	static_field("milliTime", Type::LONG, (void*) &System_millitime);
	static_field("microTime", Type::LONG, (void*) &System_microtime);
	static_field("nanoTime", Type::LONG, (void*) &System_nanotime);

	static_method("print", {
		{Type::VOID, {Type::MPZ}, (void*) &System_print_mpz, Method::NATIVE},
		{Type::VOID, {Type::MPZ_TMP}, (void*) &System_print_mpz_tmp, Method::NATIVE},
		{Type::VOID, {Type::INTEGER}, (void*) &System_print_int, Method::NATIVE},
		{Type::VOID, {Type::LONG}, (void*) &System_print_long, Method::NATIVE},
		{Type::VOID, {Type::BOOLEAN}, (void*) &System_print_bool, Method::NATIVE},
		{Type::VOID, {Type::REAL}, (void*) &System_print_float, Method::NATIVE},
		{Type::VOID, {Type::POINTER}, (void*) &System_print, Method::NATIVE}
	});
}

long get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

Compiler::value System_operations(Compiler& c) {
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(c.F, jit_type_void_ptr, (long int) &c.vm->operations);
	return {jit_insn_load_relative(c.F, jit_ops_ptr, 0, jit_type_uint), Type::LONG};
}

Compiler::value System_time(Compiler& c) {
	return c.insn_call(Type::LONG, {}, (void*) get_sec_time);
}

Compiler::value System_millitime(Compiler& c) {
	return c.insn_call(Type::LONG, {}, (void*) get_milli_time);
}

Compiler::value System_microtime(Compiler& c) {
	return c.insn_call(Type::LONG, {}, (void*) get_micro_time);
}

Compiler::value System_nanotime(Compiler& c) {
	return c.insn_call(Type::LONG, {}, (void*) get_nano_time);
}

Compiler::value System_version(Compiler& c) {
	return c.new_integer(LEEKSCRIPT_VERSION);
}

void System_print(LSValue* value) {
	value->print(*VM::current()->output);
	*VM::current()->output << std::endl;
	LSValue::delete_temporary(value);
}

void System_print_int(int v) {
	*VM::current()->output << v << std::endl;
}

void System_print_mpz(__mpz_struct v) {
	char buff[1000];
	mpz_get_str(buff, 10, &v);
	*VM::current()->output << buff << std::endl;
}
void System_print_mpz_tmp(__mpz_struct v) {
	char buff[1000];
	mpz_get_str(buff, 10, &v);
	*VM::current()->output << buff << std::endl;
	mpz_clear(&v);
	VM::current()->mpz_deleted++;
}

void System_print_long(long v) {
	*VM::current()->output << v << std::endl;
}

void System_print_bool(bool v) {
	*VM::current()->output << std::boolalpha << v << std::endl;
}

void System_print_float(double v) {
	*VM::current()->output << v << std::endl;
}

}
