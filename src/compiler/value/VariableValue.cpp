#include "VariableValue.hpp"
#include "../../vm/VM.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

VariableValue::VariableValue(std::shared_ptr<Token> token) : token(token) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
}

void VariableValue::print(ostream& os, int, bool debug) const {
	os << token->content;
	if (debug) {
		os << " " << types;
	}
}

Location VariableValue::location() const {
	return token->location;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(token.get());

	if (var != nullptr) {
		type = var->type;
		var->initial_type = type;
		scope = var->scope;
		attr_types = var->attr_types;
		if (scope != VarScope::INTERNAL and var->function != analyser->current_function()) {
			capture_index = analyser->current_function()->capture(var);
			var->index = capture_index;
			scope = VarScope::CAPTURE;
  		}
	} else {
		type = Type::POINTER;
	}

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
	if (req_type.raw_type == RawType::REAL) {
		type.raw_type = RawType::REAL;
	}

	type.temporary = false;

	//	cout << "VV " << name << " : " << type << endl;
	//	cout << "var scope : " << (int)var->scope << endl;
	//	for (auto t : attr_types)
	//	cout << t.first << " : " << t.second << endl;
	types = type;
}

bool VariableValue::will_take(SemanticAnalyser* analyser, const vector<Type>& args, int level) {
	if (var != nullptr and var->value != nullptr) {
		var->value->will_take(analyser, args, level);
		if (auto f = dynamic_cast<Function*>(var->value)) {
			if (f->versions.find(args) != f->versions.end()) {
				// std::cout << "[vv] set type from version " << args << std::endl;
				var->type = f->versions.at(args)->type;
				var->version = args;
				var->has_version = true;
			} else {
				var->type = this->type;
			}
		} else {
			var->type = this->type;
		}
		type = var->type;
		types = type;
	}
	// set_version(args);
	return false;
}

bool VariableValue::will_store(SemanticAnalyser* analyser, const Type& type) {
	if (var != nullptr and var->value != nullptr) {
		var->value->will_store(analyser, type);
		this->type = var->value->type;
		var->type = this->type;
		types = this->type;
	}
	return false;
}

void VariableValue::change_type(SemanticAnalyser*, const Type& type) {
	if (var != nullptr) {
		var->type = type;
		this->type = type;
	}
}

Type VariableValue::version_type(std::vector<Type> version) const {
	if (var != nullptr) {
		if (auto f = dynamic_cast<Function*>(var->value)) {
			return f->versions.at(version)->type;
		}
	}
	return type;
}

Compiler::value VariableValue::compile(Compiler& c) const {

	// std::cout << "Compile var " << name << " " << version << std::endl;
	// cout << "compile vv " << name << " : " << type << "(" << (int) scope << ")" << endl;
	// cout << "req type : " << req_type << endl;

	if (scope == VarScope::CAPTURE) {
		return c.insn_get_capture(capture_index, type);
	}

	jit_value_t v;
	if (scope == VarScope::INTERNAL) {
		v = c.vm->internals.at(name);
	} else if (scope == VarScope::LOCAL) {
		auto f = dynamic_cast<Function*>(var->value);
		if (has_version && f) {
			return f->compile_version(c, version);
		}
		v = c.get_var(name).v;
	} else { /* if (scope == VarScope::PARAMETER) */
		v = jit_value_get_param(c.F, 1 + var->index); // 1 offset for function ptr
	}

	if (var->type.nature != Nature::UNKNOWN and var->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return {VM::value_to_pointer(c.F, v, var->type), type};
	}
	if (var->type.raw_type == RawType::INTEGER and type.raw_type == RawType::REAL) {
		return {VM::int_to_real(c.F, v), type};
	}
	return {v, type};
}

Compiler::value VariableValue::compile_l(Compiler& c) const {

	if (scope == VarScope::CAPTURE) {
		return c.insn_address_of(c.insn_get_capture(capture_index, type));
	}

	jit_value_t v;
	// No internal values here
	if (scope == VarScope::LOCAL) {
		v = c.get_var(name).v;
	} else { /* if (scope == VarScope::PARAMETER) */
		v = jit_value_get_param(c.F, 1 + var->index); // 1 offset for function ptr
	}
	return c.insn_address_of({v, type});
}

Value* VariableValue::clone() const {
	auto vv = new VariableValue(token);
	return vv;
}

}
