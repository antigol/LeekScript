#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/Program.hpp"
#include "../../lib/utf8.h"
#include "../vm/LSValue.hpp"

using namespace std;

namespace ls {

Compiler::Compiler(VM* vm) : vm(vm) {}

Compiler::~Compiler() {}

void Compiler::enter_block() {
	variables.push_back(std::map<std::string, value> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
}

void Compiler::leave_block() {
	delete_variables_block(1);
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
}

void Compiler::delete_variables_block(int deepness) {
	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {
		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {
			insn_delete(it->second);
		}
	}
}

void Compiler::delete_function_variables() {
	for (const auto& v : function_variables.back()) {
		insn_delete(v);
	}
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::map<std::string, value> {});
	function_variables.push_back(std::vector<value> {});
	functions.push(F);
	functions_blocks.push_back(0);
	catchers.push_back({});
	this->F = F;
}

void Compiler::leave_function() {
	variables.pop_back();
	function_variables.pop_back();
	functions.pop();
	functions_blocks.pop_back();
	catchers.pop_back();
	this->F = functions.top();
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}

/*
 * Operators
 */
void Compiler::insn_store(Compiler::value a, Compiler::value b) const {
	jit_insn_store(F, a.v, b.v);
}
void Compiler::insn_store_relative(Compiler::value a, int pos, Compiler::value b) const {
	jit_insn_store_relative(F, a.v, pos, b.v);
}
Compiler::value Compiler::insn_not(Compiler::value v) const {
	return {jit_insn_not(F, v.v), v.t};
}
Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, insn_to_bool(a).v, insn_to_bool(b).v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, insn_to_bool(a).v, insn_to_bool(b).v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	auto result_type = [&]() {
		if (a.t == Type::REAL or b.t == Type::REAL) return Type::REAL;
		if (a.t == Type::LONG or b.t == Type::LONG) return Type::LONG;
		return Type::INTEGER;
	}();
	return {jit_insn_add(F, a.v, b.v), result_type};
}
Compiler::value Compiler::insn_sub(Compiler::value a, Compiler::value b) const {
	auto result_type = [&]() {
		if (a.t == Type::REAL or b.t == Type::REAL) return Type::REAL;
		if (a.t == Type::LONG or b.t == Type::LONG) return Type::LONG;
		return Type::INTEGER;
	}();
	return {jit_insn_sub(F, a.v, b.v), result_type};
}
Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	return {jit_insn_eq(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_ne(Compiler::value a, Compiler::value b) const {
	return {jit_insn_ne(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_lt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_le(Compiler::value a, Compiler::value b) const {
	return {jit_insn_le(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_gt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_ge(Compiler::value a, Compiler::value b) const {
	return {jit_insn_ge(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	return {jit_insn_mul(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_div(Compiler::value a, Compiler::value b) const {
	return {jit_insn_div(F, jit_insn_convert(F, a.v, LS_REAL, 0), b.v), Type::REAL};
}
Compiler::value Compiler::insn_int_div(Compiler::value a, Compiler::value b) const {
	return {jit_insn_div(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_bit_xor(Compiler::value a, Compiler::value b) const {
	return {jit_insn_xor(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_mod(Compiler::value a, Compiler::value b) const {
	return {jit_insn_rem(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_pow(Compiler::value a, Compiler::value b) const {
	return {jit_insn_pow(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_log10(Compiler::value a) const {
	return {jit_insn_log10(F, a.v), Type::INTEGER};
}

/*
 * Values
 */
Compiler::value Compiler::clone(Compiler::value v) const {
	if (v.t.must_manage_memory()) {
		return insn_call(v.t, {v}, +[](LSValue* value) {
			return value->clone();
		});
	}
	return v;
}
Compiler::value Compiler::new_null() const {
	return {jit_value_create_long_constant(F, LS_POINTER, (long) LSNull::get()), Type::NULLL};
}
Compiler::value Compiler::new_bool(bool b) const {
	return {LS_CREATE_BOOLEAN(F, b), Type::BOOLEAN};
}
Compiler::value Compiler::new_integer(int i) const {
	return {LS_CREATE_INTEGER(F, i), Type::INTEGER};
}
Compiler::value Compiler::new_long(long l) const {
	return {LS_CREATE_LONG(F, l), Type::LONG};
}
Compiler::value Compiler::new_pointer(const void* p) const {
	return {jit_value_create_long_constant(F, LS_POINTER, (long)(void*)(p)), Type::POINTER};
}
Compiler::value Compiler::new_mpz(long value) const {
	jit_value_t mpz_struct = jit_value_create(F, VM::mpz_type);
	jit_value_set_addressable(mpz_struct);
	auto mpz_addr = insn_address_of({mpz_struct, Type::MPZ});
	auto jit_value = new_long(value);
	insn_call(Type::VOID, {mpz_addr, jit_value}, &mpz_init_set_ui);
	VM::inc_mpz_counter(F);
	return {mpz_struct, Type::MPZ_TMP};
}
Compiler::value Compiler::new_object() const {
	return insn_call(Type::OBJECT, {}, +[]() {
		// FIXME coverage doesn't work for the one line version
		auto o = new LSObject();
		return o;
	});
}
Compiler::value Compiler::new_object_class(Compiler::value clazz) const {
	return insn_call(Type::POINTER, {clazz}, +[](LSClass* clazz) {
		return new LSObject(clazz);
	});
}

Compiler::value Compiler::to_int(Compiler::value v) const {
	if (v.t.not_temporary() == Type::MPZ) {
		auto v_addr = insn_address_of(v);
		return to_int(insn_call(Type::LONG, {v_addr}, &mpz_get_si));
	}
	return {jit_insn_convert(F, v.v, LS_INTEGER, 0), Type::INTEGER};
}

Compiler::value Compiler::to_long(Compiler::value v) const {
	if (v.t.not_temporary() == Type::MPZ) {
		auto v_addr = insn_address_of(v);
		return insn_call(Type::LONG, {v_addr}, &mpz_get_si);
	}
	return {jit_insn_convert(F, v.v, LS_LONG, 0), Type::LONG};
}

Compiler::value Compiler::insn_to_pointer(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		return v; // already a pointer
	}
	Type new_type = v.t;
	new_type.nature = Nature::POINTER;
	return {VM::value_to_pointer(F, v.v, v.t), new_type};
}

Compiler::value Compiler::insn_to_bool(Compiler::value v) const {
	if (v.t == Type::BOOLEAN) {
		return v;
	}
	if (v.t == Type::INTEGER) {
		return {jit_insn_to_bool(F, v.v), Type::BOOLEAN};
	}
	if (v.t.raw_type == RawType::STRING) {
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSString::to_bool);
	}
	if (v.t.raw_type == RawType::ARRAY) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSArray<int>::to_bool);
	}
	if (v.t.raw_type == RawType::FUNCTION) {
		return new_bool(true);
	}
	if (v.t.raw_type == RawType::MPZ) {
		// TODO
		return v;
	}
	return insn_call(Type::BOOLEAN, {v}, +[](LSValue* v) {
		return v->to_bool();
	});
}

Compiler::value Compiler::insn_address_of(Compiler::value v) const {
	return {jit_insn_address_of(F, v.v), Type::POINTER};
}

Compiler::value Compiler::insn_load(Compiler::value v, int pos, Type t) const {
	return {jit_insn_load_relative(F, v.v, pos, VM::get_jit_type(t)), t};
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	if (v.t == Type::NULLL) return new_integer(LSValue::NULLL);
	if (v.t == Type::BOOLEAN) return new_integer(LSValue::BOOLEAN);
	if (v.t.isNumber()) return new_integer(LSValue::NUMBER);
	if (v.t == Type::STRING) return new_integer(LSValue::STRING);
	if (v.t == Type::ARRAY) return new_integer(LSValue::ARRAY);
	if (v.t == Type::MAP) return new_integer(LSValue::MAP);
	if (v.t == Type::SET) return new_integer(LSValue::SET);
	if (v.t == Type::INTERVAL) return new_integer(LSValue::INTERVAL);
	if (v.t == Type::FUNCTION) return new_integer(LSValue::FUNCTION);
	if (v.t == Type::OBJECT) return new_integer(LSValue::OBJECT);
	if (v.t == Type::CLASS) return new_integer(LSValue::CLASS);
	return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
		return v->type;
	});
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	if (v.t == Type::NULLL)
		return new_pointer(vm->system_vars["Null"]);
	if (v.t == Type::BOOLEAN)
		return new_pointer(vm->system_vars["Boolean"]);
	if (v.t.isNumber())
		return new_pointer(vm->system_vars["Number"]);
	if (v.t == Type::STRING)
		return new_pointer(vm->system_vars["String"]);
	if (v.t.raw_type == RawType::ARRAY)
		return new_pointer(vm->system_vars["Array"]);
	if (v.t.raw_type == RawType::MAP)
		return new_pointer(vm->system_vars["Map"]);
	if (v.t.raw_type == RawType::SET)
		return new_pointer(vm->system_vars["Set"]);
	if (v.t.raw_type == RawType::INTERVAL)
		return new_pointer(vm->system_vars["Interval"]);
	if (v.t.raw_type == RawType::FUNCTION)
		return new_pointer(vm->system_vars["Function"]);
	if (v.t == Type::OBJECT)
		return new_pointer(vm->system_vars["Object"]);
	if (v.t == Type::CLASS)
		return new_pointer(vm->system_vars["Class"]);

	return insn_call(Type::CLASS, {v}, +[](LSValue* v) {
		return v->getClass();
	});
}

void Compiler::insn_delete(Compiler::value v) const {
	if (v.t.must_manage_memory()) {
		// insn_call(Type::VOID, {v}, (void*) &LSValue::delete_ref);
		insn_if_not(insn_native(v), [&]() {
			auto refs = insn_refs(v);
			insn_if(refs, [&]() {
				insn_if_not(insn_dec_refs(v, refs), [&]() {
					insn_call(Type::VOID, {v}, (void*) &LSValue::free);
				});
			});
		});
	} else if (v.t.not_temporary() == Type::MPZ) {
		insn_delete_mpz(v);
	}
}

Compiler::value Compiler::insn_refs(Compiler::value v) const {
	assert(v.t.must_manage_memory());
	return insn_load(v, 12, Type::INTEGER);
}

Compiler::value Compiler::insn_native(Compiler::value v) const {
	assert(v.t.must_manage_memory());
	return insn_load(v, 16, Type::BOOLEAN);
}

void Compiler::insn_delete_temporary(Compiler::value v) const {
	if (v.t.must_manage_memory()) {
		// insn_call(Type::VOID, {v}, (void*) &LSValue::delete_temporary);
		insn_if_not(insn_refs(v), [&]() {
			insn_call(Type::VOID, {v}, (void*) &LSValue::free);
		});
	} else if (v.t == Type::MPZ_TMP) {
		insn_delete_mpz(v);
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	if (v.t.not_temporary() == Type::STRING) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSString::int_size);
	} else if (v.t == Type::INT_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<int>::int_size);
	} else if (v.t == Type::REAL_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<double>::int_size);
	} else {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<LSValue*>::int_size);
	}
}

Compiler::value Compiler::insn_get_capture(int index, Type type) const {
	Compiler::value fun = {jit_value_get_param(F, 0), Type::POINTER}; // function pointer
	auto jit_index = new_integer(index);
	auto v = insn_call(Type::POINTER, {fun, jit_index}, +[](LSFunction<LSValue*>* fun, int index) {
		LSValue* v = fun->get_capture(index);
//		v->refs++;
		return v;
	});
	if (type.nature == Nature::VALUE) {
		v.v = VM::pointer_to_value(F, v.v, type);
	}
	return {v.v, type};
}

void Compiler::insn_push_array(Compiler::value array, Compiler::value value) const {
	if (array.t.getElementType() == Type::INTEGER) {
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<int>* array, int value) {
			array->push_back(value);
		});
	} else if (array.t.getElementType() == Type::REAL) {
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<double>* array, double value) {
			array->push_back(value);
		});
	} else {
		insn_call(Type::VOID, {array, value}, (void*) +[](LSArray<LSValue*>* array, LSValue* value) {
			array->push_inc(value);
		});
	}
}

Compiler::value Compiler::insn_move_inc(Compiler::value value) const {
	if (value.t.must_manage_memory()) {
		if (value.t.reference) {
			insn_inc_refs(value);
			return value;
		} else {
			return insn_call(value.t, {value}, (void*) +[](LSValue* v) {
				return v->move_inc();
			});
		}
	}
	if (value.t.temporary) {
		return value;
	}
	if (value.t == Type::MPZ) {
		return insn_clone_mpz(value);
	} else {
		return value;
	}
}

Compiler::value Compiler::insn_clone_mpz(Compiler::value mpz) const {
	jit_value_t new_mpz = jit_value_create(F, VM::mpz_type);
	jit_value_set_addressable(new_mpz);
	Compiler::value r = {new_mpz, Type::MPZ_TMP};
	auto r_addr = insn_address_of(r);
	auto mpz_addr = insn_address_of(mpz);
	insn_call(Type::VOID, {r_addr, mpz_addr}, &mpz_init_set);
	VM::inc_mpz_counter(F);
	return r;
}

void Compiler::insn_delete_mpz(Compiler::value mpz) const {
	auto mpz_addr = insn_address_of(mpz);
	insn_call(Type::VOID, {mpz_addr}, &mpz_clear);
	// Increment mpz values counter
	jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &vm->mpz_deleted);
	jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

Compiler::value Compiler::insn_inc_refs(value v) const {
	if (v.t.must_manage_memory()) {
		auto new_refs = insn_add(insn_refs(v), new_integer(1));
		insn_store_relative(v, 12, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_dec_refs(value v, value previous) const {
	if (v.t.must_manage_memory()) {
		if (previous.v == nullptr) {
			previous = insn_refs(v);
		}
		auto new_refs = insn_sub(previous, new_integer(1));
		insn_store_relative(v, 12, new_refs);
		return new_refs;
	}
	return new_integer(0);
}

Compiler::value Compiler::insn_move(Compiler::value v) const {
	if (v.t.must_manage_memory() and !v.t.temporary and !v.t.reference) {
		return insn_call(v.t, {v}, (void*) +[](LSValue* v) {
			return v->move();
		});
	}
	return v;
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, void* func) const {
	std::vector<jit_value_t> jit_args;
	std::vector<jit_type_t> arg_types;
	for (const auto& arg : args) {
		jit_args.push_back(arg.v);
		arg_types.push_back(VM::get_jit_type(arg.t));
	}
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(return_type), arg_types.data(), arg_types.size(), 1);
	Compiler::value v = {jit_insn_call_native(F, "call", func, sig, jit_args.data(), arg_types.size(), 0), return_type};
	jit_type_free(sig);
	return v;
}

void Compiler::function_add_capture(Compiler::value fun, Compiler::value capture) {
	insn_call(Type::VOID, {fun, capture}, +[](LSFunction<LSValue*>* fun, LSValue* cap) {
		fun->add_capture(cap);
	});
}

// Debug-only function
// LCOV_EXCL_START
void Compiler::log(const std::string&& str) const {
	auto s = new_pointer(new std::string(str));
	insn_call(Type::VOID, {s}, +[](std::string* s) {
		std::cout << *s << std::endl;
		delete s;
	});
}
// LCOV_EXCL_STOP

/*
 * Iterators
 */
Compiler::value Compiler::iterator_begin(Compiler::value v) const {
	if (v.t.raw_type == RawType::ARRAY) {
		Compiler::value it = {jit_value_create(F, VM::get_jit_type(v.t)), v.t};
		insn_store(it, insn_load(v, 24));
		return it;
	}
	if (v.t.raw_type == RawType::INTERVAL) {
		jit_type_t types[2] = {jit_type_void_ptr, jit_type_int};
		auto interval_iterator = jit_type_create_struct(types, 2, 1);
		Compiler::value it = {jit_value_create(F, interval_iterator), Type::INTERVAL_ITERATOR};
		jit_type_free(interval_iterator);
		auto addr = insn_address_of(it);
		jit_insn_store_relative(F, addr.v, 0, v.v);
		jit_insn_store_relative(F, addr.v, 8, insn_load(v, 20, Type::INTEGER).v);
		return it;
	}
	if (v.t.raw_type == RawType::STRING) {
		jit_type_t types[5] = {jit_type_void_ptr, jit_type_int, jit_type_int, jit_type_int, jit_type_int};
		auto string_iterator = jit_type_create_struct(types, 5, 1);
		Compiler::value it = {jit_value_create(F, string_iterator), Type::STRING_ITERATOR};
		jit_type_free(string_iterator);
		auto addr = insn_address_of(it);
		insn_call(Type::VOID, {v, addr}, (void*) +[](LSString* str, LSString::iterator* it) {
			auto i = LSString::iterator_begin(str);
			it->buffer = i.buffer;
			it->index = 0;
			it->pos = 0;
			it->next_pos = 0;
			it->character = 0;
		});
		return it;
	}
	if (v.t.raw_type == RawType::MAP) {
		return insn_load(v, 48, v.t);
	}
	if (v.t.raw_type == RawType::SET) {
		jit_type_t types[2] = {jit_type_void_ptr, jit_type_int};
		auto set_iterator = jit_type_create_struct(types, 2, 1);
		Compiler::value it = {jit_value_create(F, set_iterator), Type::SET_ITERATOR};
		jit_type_free(set_iterator);
		auto addr = insn_address_of(it);
		jit_insn_store_relative(F, addr.v, 0, insn_load(v, 48, v.t).v);
		jit_insn_store_relative(F, addr.v, 8, new_integer(0).v);
		return it;
	}
	if (v.t == Type::INTEGER) {
		jit_type_t types[3] = {jit_type_int, jit_type_int, jit_type_int};
		auto integer_iterator = jit_type_create_struct(types, 3, 1);
		Compiler::value it = {jit_value_create(F, integer_iterator), Type::INTEGER_ITERATOR};
		jit_type_free(integer_iterator);
		auto addr = jit_insn_address_of(F, it.v);
		jit_insn_store_relative(F, addr, 0, v.v);
		jit_insn_store_relative(F, addr, 4, to_int(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		jit_insn_store_relative(F, addr, 8, new_integer(0).v);
		return it;
	}
	if (v.t == Type::LONG) {
		jit_type_t types[3] = {jit_type_long, jit_type_long, jit_type_int};
		auto long_iterator = jit_type_create_struct(types, 3, 1);
		Compiler::value it = {jit_value_create(F, long_iterator), Type::LONG_ITERATOR};
		jit_type_free(long_iterator);
		auto addr = jit_insn_address_of(F, it.v);
		jit_insn_store_relative(F, addr, 0, v.v);
		jit_insn_store_relative(F, addr, 8, to_long(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		jit_insn_store_relative(F, addr, 16, new_long(0).v);
		return it;
	}
	if (v.t == Type::MPZ) {
		jit_type_t types[3] = {VM::mpz_type, VM::mpz_type, jit_type_int};
		auto mpz_iterator = jit_type_create_struct(types, 3, 1);
		Compiler::value it = {jit_value_create(F, mpz_iterator), Type::MPZ_ITERATOR};
		jit_type_free(mpz_iterator);
		auto addr = jit_insn_address_of(F, it.v);
		jit_insn_store_relative(F, addr, 0, v.v);
		auto l =
		jit_insn_store_relative(F, addr, 16, to_long(insn_pow(new_integer(10), to_int(insn_log10(v)))).v);
		jit_insn_store_relative(F, addr, 32, new_long(0).v);
		return it;
	}
}

Compiler::value Compiler::iterator_end(Compiler::value v, Compiler::value it) const {
	if (v.t.raw_type == RawType::ARRAY) {
		return insn_eq(it, insn_load(v, 32));
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto interval = insn_load(addr, 0, Type::POINTER);
		auto end = insn_load(interval, 24, Type::INTEGER);
		auto pos = insn_load(addr, 8, Type::INTEGER);
		return insn_gt(pos, end);
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_call(Type::BOOLEAN, {addr}, &LSString::iterator_end);
	}
	if (v.t.raw_type == RawType::MAP) {
		auto end = insn_add(v, new_integer(32)); // end_ptr = &map + 24
		return insn_eq(it, end);
	}
	if (it.t == Type::SET_ITERATOR) {
		auto addr = insn_address_of(it);
		auto ptr = insn_load(addr, 0, Type::POINTER);
		auto end = insn_add(v, new_integer(32)); // end_ptr = &set + 24
		return insn_eq(ptr, end);
	}
	if (it.t == Type::INTEGER_ITERATOR) {
		auto addr = insn_address_of(it);
		auto p = insn_load(addr, 4, Type::INTEGER);
		return insn_eq(p, new_integer(0));
	}
	if (it.t == Type::LONG_ITERATOR) {
		auto addr = insn_address_of(it);
		auto p = insn_load(addr, 8, Type::LONG);
		return insn_eq(p, new_integer(0));
	}
}

Compiler::value Compiler::iterator_key(Compiler::value v, Compiler::value it, Compiler::value previous) const {
	if (it.t.raw_type == RawType::ARRAY) {
		return insn_int_div(insn_sub(it, insn_load(v, 24)), new_integer(it.t.element().size() / 8));
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto interval = insn_load(addr, 0);
		auto start = insn_load(interval, 20);
		auto e = insn_load(addr, 8, Type::INTEGER);
		return insn_sub(e, start);
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_call(Type::INTEGER, {addr}, &LSString::iterator_key);
	}
	if (it.t.raw_type == RawType::MAP) {
		if (previous.t.must_manage_memory()) {
			insn_call(Type::VOID, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto key = insn_load(it, 32, it.t.getKeyType());
		insn_inc_refs(key);
		return key;
	}
	if (it.t == Type::SET_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_load(addr, 8, Type::INTEGER);
	}
	if (it.t == Type::INTEGER_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_load(addr, 8, Type::INTEGER);
	}
	if (it.t == Type::LONG_ITERATOR) {
		auto addr = insn_address_of(it);
		return insn_load(addr, 16, Type::INTEGER);
	}
}

Compiler::value Compiler::iterator_get(Compiler::value it, Compiler::value previous) const {
	if (it.t.raw_type == RawType::ARRAY) {
		if (previous.t.must_manage_memory()) {
			insn_call(Type::VOID, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto e = insn_load(it, 0, it.t.getElementType());
		insn_inc_refs(e);
		return e;
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto e = insn_load(addr, 8, Type::INTEGER);
		return e;
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		auto int_char = insn_call(Type::INTEGER, {addr}, &LSString::iterator_get);
		return insn_call(Type::STRING, {int_char, previous}, (void*) +[](unsigned int c, LSString* previous) {
			if (previous != nullptr) {
				LSValue::delete_ref(previous);
			}
			char dest[5];
			u8_toutf8(dest, 5, &c, 1);
			auto s = new LSString(dest);
			s->refs = 1;
			return s;
		});
	}
	if (it.t.raw_type == RawType::MAP) {
		if (previous.t.must_manage_memory()) {
			insn_call(Type::VOID, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto e = insn_load(it, 32 + 8, it.t.element());
		insn_inc_refs(e);
		return e;
	}
	if (it.t == Type::SET_ITERATOR) {
		if (previous.t.must_manage_memory()) {
			insn_call(Type::VOID, {previous}, +[](LSValue* previous) {
				if (previous != nullptr)
					LSValue::delete_ref(previous);
			});
		}
		auto addr = insn_address_of(it);
		auto ptr = insn_load(addr, 0, Type::POINTER);
		auto e = insn_load(ptr, 32, previous.t);
		insn_inc_refs(e);
		return e;
	}
	if (it.t == Type::INTEGER_ITERATOR) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::INTEGER);
		auto p = insn_load(addr, 4, Type::INTEGER);
		return insn_int_div(n, p);
	}
	if (it.t == Type::LONG_ITERATOR) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::LONG);
		auto p = insn_load(addr, 8, Type::LONG);
		return insn_int_div(n, p);
	}
}

void Compiler::iterator_increment(Compiler::value it) const {
	if (it.t.raw_type == RawType::ARRAY) {
		insn_store(it, insn_add(it, new_integer(it.t.element().size() / 8)));
		//insn_store(it, insn_add(it, insn_mul(new_integer(16), new_integer(it.t.element().size() / 8)) ));
		return;
	}
	if (it.t == Type::INTERVAL_ITERATOR) {
		auto addr = insn_address_of(it);
		auto pos = insn_load(addr, 8, Type::INTEGER);
		insn_store_relative(addr, 8, insn_add(pos, new_integer(1)));
		return;
	}
	if (it.t == Type::STRING_ITERATOR) {
		auto addr = insn_address_of(it);
		insn_call(Type::VOID, {addr}, &LSString::iterator_next);
		return;
	}
	if (it.t.raw_type == RawType::MAP) {
		insn_store(it, insn_call(Type::POINTER, {it}, (void*) +[](LSMap<int, int>::iterator it) {
			it++;
			return it;
		}));
		return;
	}
	if (it.t == Type::SET_ITERATOR) {
		auto addr = insn_address_of(it);
		auto ptr = insn_load(addr, 0, Type::POINTER);
		insn_store_relative(addr, 8, insn_add(insn_load(addr, 8, Type::INTEGER), new_integer(1)));
		insn_store_relative(addr, 0, insn_call(Type::POINTER, {ptr}, (void*) +[](LSSet<int>::iterator it) {
			it++;
			return it;
		}));
		return;
	}
	if (it.t == Type::INTEGER_ITERATOR) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::INTEGER);
		auto p = insn_load(addr, 4, Type::INTEGER);
		auto i = insn_load(addr, 8, Type::INTEGER);
		jit_insn_store_relative(F, addr.v, 0, insn_mod(n, p).v);
		jit_insn_store_relative(F, addr.v, 4, insn_int_div(p, new_integer(10)).v);
		jit_insn_store_relative(F, addr.v, 8, insn_add(i, new_integer(1)).v);
		return;
	}
	if (it.t == Type::LONG_ITERATOR) {
		auto addr = insn_address_of(it);
		auto n = insn_load(addr, 0, Type::LONG);
		auto p = insn_load(addr, 8, Type::LONG);
		auto i = insn_load(addr, 16, Type::INTEGER);
		jit_insn_store_relative(F, addr.v, 0, insn_mod(n, p).v);
		jit_insn_store_relative(F, addr.v, 8, insn_int_div(p, new_integer(10)).v);
		jit_insn_store_relative(F, addr.v, 16, insn_add(i, new_integer(1)).v);
		return;
	}
}

/*
 * Controls
 */
void Compiler::insn_if(Compiler::value condition, std::function<void()> then) const {
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, condition.v, &label_end);
	then();
	jit_insn_label(F, &label_end);
}

void Compiler::insn_if_not(Compiler::value condition, std::function<void()> then) const {
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if(F, condition.v, &label_end);
	then();
	jit_insn_label(F, &label_end);
}

void Compiler::insn_throw(Compiler::value v) const {
	jit_insn_throw(F, v.v);
}

void Compiler::insn_throw_object(vm::Exception type) const {
	auto t = new_integer(type);
	auto ex = insn_call(Type::POINTER, {t}, &VM::get_exception_object<0>);
	insn_throw(ex);
}

/*
 * Variables
 */
void Compiler::add_var(const std::string& name, jit_value_t value, const Type& type, bool ref) {
	Type t = type;
	t.reference = ref;
	variables.back()[name] = {value, t};
}

void Compiler::add_function_var(jit_value_t value, const Type& type) {
	function_variables.back().push_back({value, type});
}

Compiler::value& Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	return *((Compiler::value*) nullptr); // Should not reach this line
}

void Compiler::set_var_type(std::string& name, const Type& type) {
	variables.back()[name].t = type;
}

void Compiler::update_var(std::string& name, jit_value_t value, const Type& type) {
	variables.back()[name].v = value;
	variables.back()[name].t = type;
}

void Compiler::enter_loop(jit_label_t* end_label, jit_label_t* cond_label) {
	loops_end_labels.push_back(end_label);
	loops_cond_labels.push_back(cond_label);
	loops_blocks.push_back(0);
}

void Compiler::leave_loop() {
	loops_end_labels.pop_back();
	loops_cond_labels.pop_back();
	loops_blocks.pop_back();
}

jit_label_t* Compiler::get_current_loop_end_label(int deepness) const {
	return loops_end_labels[loops_end_labels.size() - deepness];
}

jit_label_t* Compiler::get_current_loop_cond_label(int deepness) const {
	return loops_cond_labels[loops_cond_labels.size() - deepness];
}

int Compiler::get_current_loop_blocks(int deepness) const {
	int sum = 0;
	for (size_t i = loops_blocks.size() - deepness; i < loops_blocks.size(); ++i) {
		sum += loops_blocks[i];
	}
	return sum;
}

void Compiler::inc_ops(int amount) {
	inc_ops_jit(new_integer(amount));
}

void Compiler::inc_ops_jit(Compiler::value amount) {
	// Operations enabled?
	if (not vm->enable_operations) return;

	// Variable counter pointer
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, LS_POINTER, (long int) &vm->operations);

	// Increment counter
	jit_value_t jit_ops = jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);
	jit_insn_store_relative(F, jit_ops_ptr, 0, jit_insn_add(F, jit_ops, amount.v));

	// Compare to the limit
	jit_value_t compare = jit_insn_gt(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, vm->operation_limit));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, compare, &label_end);

	// If greater than the limit, throw exception
	insn_throw_object(vm::Exception::OPERATION_LIMIT_EXCEEDED);

	// End
	jit_insn_label(F, &label_end);
}

void Compiler::add_catcher(jit_label_t start, jit_label_t end, jit_label_t handler) {
	catchers.back().push_back({start, end, handler, jit_label_undefined});
}

void Compiler::insn_check_args(std::vector<Compiler::value> args, std::vector<LSValueType> types) const {
	for (size_t i = 0; i < args.size(); ++i) {
		auto arg = args[i];
		auto type = types[i];
		if (arg.t.nature != Nature::VALUE and type != 0) {
			auto type = types[i];
			insn_if(insn_ne(insn_typeof(arg), new_integer(type)), [&]() {
				for (auto& a : args) {
					insn_delete_temporary(a);
				}
				insn_throw_object(vm::Exception::WRONG_ARGUMENT_TYPE);
			});
		}
	}
}

}
