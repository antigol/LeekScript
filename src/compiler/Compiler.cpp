#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/Program.hpp"

using namespace std;

namespace ls {

Compiler::Compiler(Program* program) : program(program) {}

Compiler::~Compiler() {}

void Compiler::enter_block() {
	variables.push_back(std::map<std::string, CompilerVar> {});
	if (!loops_blocks.empty()) {
		loops_blocks.back()++;
	}
	functions_blocks.back()++;
}

void Compiler::leave_block(jit_function_t F) {
	delete_variables_block(F, 1);
	variables.pop_back();
	if (!loops_blocks.empty()) {
		loops_blocks.back()--;
	}
	functions_blocks.back()--;
}

void Compiler::delete_variables_block(jit_function_t F, int deepness) {

	for (int i = variables.size() - 1; i >= (int) variables.size() - deepness; --i) {

		for (auto it = variables[i].begin(); it != variables[i].end(); ++it) {

//			std::cout << "delete " << var.first  << std::endl;

			if (it->second.reference == true) {
				continue;
			}
			if (it->second.type.must_manage_memory()) {
				VM::delete_ref(F, it->second.value);
			}
			if (it->second.type == Type::GMP_INT_TMP) {
				VM::delete_gmp_int(F, it->second.value);
			}
			if (it->second.type == Type::GMP_INT) {
				VM::delete_gmp_int(F, it->second.value);
			}
		}
	}
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::map<std::string, CompilerVar> {});
	functions.push(F);
	functions_blocks.push_back(0);
	this->F = F;
}

void Compiler::leave_function() {
	variables.pop_back();
	functions.pop();
	functions_blocks.pop_back();
	this->F = functions.top();
}

int Compiler::get_current_function_blocks() const {
	return functions_blocks.back();
}

/*
 * Utils
 */
Compiler::value Compiler::signed_int(Compiler::value v) const {
	return {jit_insn_convert(F, v.v, jit_type_int, 0), Type::INTEGER};
}

/*
 * Operators
 */
Compiler::value Compiler::insn_not(Compiler::value v) const {
	return {jit_insn_not(F, v.v), v.t};
}
Compiler::value Compiler::insn_and(Compiler::value a, Compiler::value b) const {
	return {jit_insn_and(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_or(Compiler::value a, Compiler::value b) const {
	return {jit_insn_or(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_add(Compiler::value a, Compiler::value b) const {
	return {jit_insn_add(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::insn_eq(Compiler::value a, Compiler::value b) const {
	return {jit_insn_eq(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_lt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_lt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_gt(Compiler::value a, Compiler::value b) const {
	return {jit_insn_gt(F, a.v, b.v), Type::BOOLEAN};
}
Compiler::value Compiler::insn_mul(Compiler::value a, Compiler::value b) const {
	return {jit_insn_mul(F, a.v, b.v), Type::INTEGER};
}
Compiler::value Compiler::new_mpz() const {
	return {VM::create_gmp_int(F, 0), Type::GMP_INT};
}

/*
 * Values
 */
Compiler::value Compiler::new_null() const {
	return {LS_CREATE_POINTER(F, LSNull::get()), Type::NULLL};
}
Compiler::value Compiler::new_bool(bool b) const {
	return {LS_CREATE_BOOLEAN(F, b), Type::BOOLEAN};
}
Compiler::value Compiler::new_integer(int i) const {
	return {LS_CREATE_INTEGER(F, i), Type::INTEGER};
}
Compiler::value Compiler::new_pointer(void* p) const {
	return {LS_CREATE_POINTER(F, p), Type::POINTER};
}
Compiler::value Compiler::insn_to_pointer(Compiler::value v) const {
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
	if (v.t == Type::STRING) {
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSString::isTrue);
	}
	if (v.t.raw_type == RawType::ARRAY) {
		// Always take LSArray<int>, but the array is not necessarily of this type
		return insn_call(Type::BOOLEAN, {v}, (void*) &LSArray<int>::isTrue);
	}
	if (v.t == Type::FUNCTION_P) {
		return new_bool(true);
	}
	// TODO other types
	return v;
}
Compiler::value Compiler::insn_to_not_bool(Compiler::value v) const {
	return {jit_insn_to_not_bool(F, insn_to_bool(v).v), Type::BOOLEAN};
}

Compiler::value Compiler::insn_address_of(Compiler::value v) const {
	return {jit_insn_address_of(F, v.v), Type::POINTER};
}

Compiler::value Compiler::insn_typeof(Compiler::value v) const {
	if (v.t == Type::NULLL) return new_integer(1);
	if (v.t == Type::BOOLEAN) return new_integer(2);
	if (v.t.isNumber()) return new_integer(3);
	if (v.t == Type::STRING) return new_integer(4);
	if (v.t == Type::ARRAY or v.t == Type::INTERVAL) return new_integer(5);
	if (v.t == Type::MAP) return new_integer(6);
	if (v.t == Type::SET) return new_integer(7);
	if (v.t == Type::FUNCTION) return new_integer(8);
	if (v.t == Type::OBJECT) return new_integer(9);
	if (v.t == Type::CLASS) return new_integer(10);
	return insn_call(Type::INTEGER, {v}, +[](LSValue* v) {
		return v->typeID();
	});
}

Compiler::value Compiler::insn_class_of(Compiler::value v) const {
	if (v.t == Type::NULLL)
		return new_pointer(program->system_vars["Null"]);
	if (v.t == Type::BOOLEAN)
		return new_pointer(program->system_vars["Boolean"]);
	if (v.t.isNumber())
		return new_pointer(program->system_vars["Number"]);
	if (v.t == Type::STRING)
		return new_pointer(program->system_vars["String"]);
	if (v.t.raw_type == RawType::ARRAY or v.t == Type::INTERVAL)
		return new_pointer(program->system_vars["Array"]);
	if (v.t.raw_type == RawType::MAP)
		return new_pointer(program->system_vars["Map"]);
	if (v.t.raw_type == RawType::SET)
		return new_pointer(program->system_vars["Set"]);
	if (v.t.raw_type == RawType::FUNCTION)
		return new_pointer(program->system_vars["Function"]);
	if (v.t == Type::OBJECT)
		return new_pointer(program->system_vars["Object"]);
	if (v.t == Type::CLASS)
		return new_pointer(program->system_vars["Class"]);
	return insn_call(Type::CLASS, {v}, +[](LSValue* v) {
		return v->getClass();
	});
}

void Compiler::insn_delete(Compiler::value v) const {
	if (v.t.nature == Nature::POINTER) {
		insn_call(Type::VOID, {v}, (void*) &LSValue::delete_temporary);
	}
}

Compiler::value Compiler::insn_array_size(Compiler::value v) const {
	if (v.t == Type::INT_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<int>::int_size);
	} else if (v.t == Type::REAL_ARRAY) {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<double>::int_size);
	} else {
		return insn_call(Type::INTEGER, {v}, (void*) &LSArray<LSValue*>::int_size);
	}
}

Compiler::value Compiler::insn_get_capture(int index, Type type) const {
	jit_value_t fun = jit_value_get_param(F, 0); // function pointer
	jit_value_t v =	VM::function_get_capture(F, fun, index);
	if (type.nature == Nature::VALUE) {
		v = VM::pointer_to_value(F, v, type);
	}
	return {v, type};
}

Compiler::value Compiler::insn_call(Type return_type, std::vector<Compiler::value> args, void* func) const {
	std::vector<jit_value_t> jit_args;
	std::vector<jit_type_t> arg_types;
	for (const auto& arg : args) {
		jit_args.push_back(arg.v);
		arg_types.push_back(VM::get_jit_type(arg.t));
	}
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(return_type), arg_types.data(), arg_types.size(), 0);
	return {jit_insn_call_native(F, "call", func, sig, jit_args.data(), arg_types.size(), 0), return_type};
}

/*
 * Variables
 */
void Compiler::add_var(const std::string& name, jit_value_t value, const Type& type, bool ref) {
	variables.back()[name] = {value, type, ref};
}

CompilerVar& Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].find(name);
		if (it != variables[i].end()) {
			return it->second;
		}
	}
	return *((CompilerVar*) nullptr); // Should not reach this line
}

void Compiler::set_var_type(std::string& name, const Type& type) {
	variables.back()[name].type = type;
}

/*
void Compiler::set_var_type(std::string& name, const Type& type) {
	variables.back().at(name).type = type;
}
*/

/*
std::map<std::string, CompilerVar> Compiler::get_vars() {
	return variables.back();
}
*/

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

}
