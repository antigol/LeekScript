#include "../../compiler/value/VariableValue.hpp"

#include "../../vm/VM.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

VariableValue::VariableValue(Token* token) {
	this->name = token->content;
	this->token = token;
	this->var = nullptr;
	constant = false;
}

VariableValue::~VariableValue() {}

void VariableValue::print(ostream& os, int, bool debug) const {
	os << token->content;
	if (debug) {
		os << " " << type;
	}
}

unsigned VariableValue::line() const {
	return token->line;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(token);
	if (var != nullptr) {
		type = var->type;
		scope = var->scope;
		attr_types = var->attr_types;
		if (var->function != analyser->current_function()) {
			capture_index = analyser->current_function()->capture(var);
			std::cout << "Capture " << var->name << " : " << capture_index << std::endl;
			type.nature = Nature::POINTER;
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
//		cout << t.first << " : " << t.second << endl;
}

bool VariableValue::will_take(SemanticAnalyser* analyser, const vector<Type>& arg_types) {

	if (var != nullptr and var->value != nullptr) {
		var->value->will_take(analyser, arg_types);
		this->type = var->value->type;
	}
	return false;
}

bool VariableValue::will_store(SemanticAnalyser* analyser, const Type& type) {
	if (var != nullptr and var->value != nullptr) {
		var->value->will_store(analyser, type);
		this->type = var->value->type;
	}
	return false;
}

void VariableValue::must_return(SemanticAnalyser* analyser, const Type& ret_type) {

	var->value->must_return(analyser, ret_type);
	type.setReturnType(ret_type);
}

/*
 * let a = 12 // integer
 * a = 5.6 // float
 * a = 12 // integer
 * a += 1.2 // float at the previous assignment
 */
void VariableValue::change_type(SemanticAnalyser*, const Type& type) {
	if (var != nullptr) {
		var->type = type;
		this->type = type;
	}
}

extern map<string, jit_value_t> internals;

jit_value_t VariableValue::compile(Compiler& c) const {

//	cout << "compile vv " << name->content << " : " << type << endl;
//	cout << "req type : " << req_type << endl;

	jit_value_t v;

	if (scope == VarScope::INTERNAL) {

		v = internals[name];

	} else if (scope == VarScope::LOCAL) {

		v = c.get_var(name).value;

	} else if (scope == VarScope::PARAMETER) {

		v = jit_value_get_param(c.F, 1 + var->index); // 1 offset for function ptr

	} else if (scope == VarScope::CAPTURE) {

		jit_value_t fun = jit_value_get_param(c.F, 0); // function pointer
		return VM::function_get_capture(c.F, fun, capture_index);
	}

	if (var->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return VM::value_to_pointer(c.F, v, var->type);
	}
	if (var->type.raw_type == RawType::INTEGER and type.raw_type == RawType::REAL) {
		return VM::int_to_real(c.F, v);
	}
	return v;
}

jit_value_t VariableValue::compile_l(Compiler& c) const {
	return compile(c);
}

}
