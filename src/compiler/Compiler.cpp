#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/LSValue.hpp"
#include "../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

Compiler::Compiler() {}

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
	variables.back().at(name).type = type;
}

std::map<std::string, CompilerVar> Compiler::get_vars() {
	return variables.back();
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

jit_value_t Compiler::call_native(jit_function_t F, jit_type_t return_type, std::vector<jit_type_t> args_type, void* function, std::vector<jit_value_t> args)
{
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, return_type, args_type.data(), args_type.size(), 0);
	jit_value_t res = jit_insn_call_native(F, "", (void*) function, sig, args.data(), args.size(), JIT_CALL_NOTHROW);
	jit_type_free(sig);
	return res;
}

bool CP_eq(LSValue* x, LSValue* y) {
	if (x == nullptr) return y == nullptr;
	if (y == nullptr) return false;
	return *x == *y;
}
bool CP_ne(LSValue* x, LSValue* y) {
	if (x == nullptr) return y != nullptr;
	if (y == nullptr) return true;
	return *x != *y;
}
bool CP_lt(LSValue* x, LSValue* y) {
	if (y == nullptr) return false;
	if (x == nullptr) return true;
	return *x < *y;
}
bool CP_le(LSValue* x, LSValue* y) {
	if (x == nullptr) return true;
	if (y == nullptr) return false;
	return *x <= *y;
}
bool CP_gt(LSValue* x, LSValue* y) {
	if (x == nullptr) return false;
	if (y == nullptr) return true;
	return *x > *y;
}
bool CP_ge(LSValue* x, LSValue* y) {
	if (y == nullptr) return true;
	if (x == nullptr) return false;
	return *x >= *y;
}

//  a?        a          a
// null < bool,number < text < vec < map < set < function < tuple

jit_value_t Compiler::compile_eq(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_eq(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_eq(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_eq, { v1, v2 });
	}
	assert(0);
	return nullptr;
}

jit_value_t Compiler::compile_ne(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_ne(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_ne(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_ne, { v1, v2 });
	}
	assert(0);
	return nullptr;
}

jit_value_t Compiler::compile_lt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_lt(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_lt(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_lt, { v1, v2 });
	}
	assert(0);
	return nullptr;
}

jit_value_t Compiler::compile_le(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_le(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_le(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_le, { v1, v2 });
	}
	assert(0);
	return nullptr;
}

jit_value_t Compiler::compile_gt(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_gt(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_gt(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_gt, { v1, v2 });
	}
	assert(0);
	return nullptr;
}

jit_value_t Compiler::compile_ge(jit_function_t F, jit_value_t v1, const Type& t1, jit_value_t v2, const Type& t2)
{
	if (Type::intersection(t1, Type::VALUE_NUMBER) && Type::intersection(t2, Type::VALUE_NUMBER)) return jit_insn_ge(F, v1, v2);
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) return jit_insn_ge(F, v1, v2);
	if (t1.raw_type->nature() == Nature::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) {
		return call_native(F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) CP_ge, { v1, v2 });
	}
	assert(0);
	return nullptr;
}


jit_value_t Compiler::compile_convert(jit_function_t F, jit_value_t v, const Type& t_in, const Type& t_out)
{
	if (t_in == t_out || v == nullptr) return v;
	if (t_out == Type::VAR) {
		return VM::value_to_lsvalue(F, v, t_in);
	}
	if (t_in == Type::I32 && t_out == Type::F64) {
		return jit_insn_convert(F, v, t_out.jit_type(), 0);
	}
	if (t_in == Type::BOOLEAN && t_out == Type::I32) {
		return v;
	}

	// TODO add other possibilities here and in Type.cpp

	assert(0);
	return nullptr;
}

}
