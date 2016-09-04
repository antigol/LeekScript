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
	if (debug) {
		os << " " << type;
	}
}

unsigned VariableValue::line() const {
	return token->line;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	var = analyser->get_var(token);
	if (var) {
		if (var->function != analyser->current_function()) {
			analyser->current_function()->capture(var);
  		}
		type = var->type;

		if (!var->type.match_with_generic(req_type)) {
			type = Type::get_compatible_type(var->type, req_type);
			type.make_it_complete();
			if (!type.match_with_generic(req_type)) {
				stringstream oss;
				print(oss, 0, false);
				analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
			}
		}
	}
	assert(type.is_complete() || !analyser->errors.empty());
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
