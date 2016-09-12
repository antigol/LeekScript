#include "Compiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/LSValue.hpp"
#include "../vm/value/LSVar.hpp"
#include "jit/jit_general.hpp"

using namespace std;

namespace ls {

Compiler::Compiler() {}

Compiler::~Compiler() {}

void Compiler::enter_block() {
	variables.push_back(std::multimap<std::string, CompilerVar>());
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

			if (it->second.reference == true) {
				continue;
			}

			jit_general::delete_ref(F, it->second.value, it->second.type);
		}
	}
}

void Compiler::enter_function(jit_function_t F) {
	variables.push_back(std::multimap<std::string, CompilerVar>());
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
	variables.back().insert(pair<string, CompilerVar>(name, {value, type, ref}));
}

CompilerVar& Compiler::get_var(const std::string& name) {
	for (int i = variables.size() - 1; i >= 0; --i) {
		auto it = variables[i].equal_range(name);
		if (it.first != it.second) {
			auto i = it.second;
			--i;
			return i->second;
		}
	}
	return *((CompilerVar*) nullptr); // Should not reach this line
}

std::multimap<std::string, CompilerVar> Compiler::get_vars() {
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

}
