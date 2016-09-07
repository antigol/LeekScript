#include "VariableValue.hpp"
#include "Function.hpp"
#include "../../vm/VM.hpp"
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
//	if (debug) {
//		os << " " << type;
//	}
}

unsigned VariableValue::line() const {
	return token->line;
}

void VariableValue::preanalyse(SemanticAnalyser* analyser)
{
	var = analyser->get_var(token);
	if (var == nullptr) return; // the error is generated by get_var

	left_type = var->type;
	type = var->type.image_conversion();
}

void VariableValue::will_take(SemanticAnalyser* analyser, const Type& req_type)
{
	if (var == nullptr) return;

	Type tmp;
	if (!Type::intersection(var->type, req_type, &tmp)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}

	if (tmp != var->type) {
		var->type = tmp;

		if (var->scope == VarScope::LOCAL && var->vd) {
			var->vd->var_type = var->type;
		}
		if (var->scope == VarScope::PARAMETER) {
			var->function->type.arguments_types[var->index] = var->type;
		}

		left_type = var->type;
		type = var->type.image_conversion();
	}
}

void VariableValue::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (var == nullptr) return;


	// Example input
	// req_type == var
	// var.type == UNKNOWN
	// type == UNKNOWN

	// Ideal output
	// type == var
	// var.type == i32|f64|var

	cout << "VV wr type=" << type << " req=" << req_type << " var->type=" << var->type << endl;

	// update type in case var->type has changed
	if (!Type::intersection(type, var->type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}

	if (!Type::intersection(type.fiber_conversion(), var->type, &var->type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}

	// save otherwise the type will be lost for the analyse
	if (var->scope == VarScope::LOCAL && var->vd) {
		if (var->vd->expression) {
			var->vd->expression->will_require(analyser, var->type);
			if (!Type::intersection(var->type, var->vd->expression->type, &var->type)) {
				add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
			}
			if (!Type::intersection(type, var->type, &type)) {
				add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
			}
		}
		var->vd->var_type = var->type;
	}
	if (var->scope == VarScope::PARAMETER) {
		var->function->type.arguments_types[var->index] = var->type;
	}

	left_type = var->type;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	var = analyser->get_var(token);
	if (var == nullptr) return;

	if (!Type::intersection(type, var->type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}
	type.make_it_pure();
	left_type = var->type;
}

extern map<string, jit_value_t> internals;

jit_value_t VariableValue::compile(Compiler& c) const
{
	jit_value_t v;
	switch (var->scope) {
		case VarScope::INTERNAL:
			v = internals[name];
			break;
		case VarScope::LOCAL:
			v = c.get_var(name).value;
			break;
		case VarScope::PARAMETER:
			v = jit_value_get_param(c.F, var->index);
			break;
	}

	return Compiler::compile_convert(c.F, v, var->type, type);
}

jit_value_t VariableValue::compile_l(Compiler& c) const
{
	if (var->type != type) return nullptr;
	jit_value_t v = compile(c);
	return jit_insn_address_of(c.F, v);
}

}
