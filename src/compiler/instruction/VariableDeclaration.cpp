#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../value/Reference.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	expression = nullptr;
	typeName = nullptr;
	var_type = Type::UNKNOWN;
}

VariableDeclaration::~VariableDeclaration() {
	delete expression;
	delete typeName;
}

void VariableDeclaration::print(ostream& os, int indent, bool debug) const {

	os << (global ? "global " : "let ");
	os << variable->content;
	if (typeName) {
		os << " : ";
		typeName->print(os);
	}
	if (debug) os << " " << var_type;
	if (expression) {
		os << " = ";
		expression->print(os, indent, debug);
	}
}

unsigned VariableDeclaration::line() const
{
	return 0;
}

void VariableDeclaration::analyse_help(SemanticAnalyser* analyser)
{
	var_type = Type::UNKNOWN;

	if (typeName) {
		var_type = typeName->getInternalType(analyser);
	}
	if (expression) {
		expression->analyse(analyser);
		if (var_type != Type::UNKNOWN) {
			expression->reanalyse(analyser, var_type);
		}
		var_type = expression->type;
	}

	var = analyser->add_var(variable, var_type, analyser->current_block(), this);
	type = Type::VOID;
}

void VariableDeclaration::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (!Type::intersection(var_type, var->type, &var_type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}
	var->type = var_type;

	if (expression) {
		expression->reanalyse(analyser, var_type);
		if (!Type::intersection(var_type, expression->type, &var_type)) {
			add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
		}
		var->type = var_type; // TODO should work only with var->type  ==>  must before remove analyser stuff in finalize
	}
}

void VariableDeclaration::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (expression) {
		expression->finalize(analyser, var_type);
		var_type = expression->type;
	} else {
		var_type.make_it_pure();
	}

	analyser->add_var(variable, var_type, analyser->current_block(), this);
}

jit_value_t VariableDeclaration::compile(Compiler& c) const
{
	jit_value_t var = jit_value_create(c.F, var_type.jit_type());
	c.add_var(variable->content, var, var_type, false);

	if (expression) {
		jit_value_t val = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			val = VM::move_inc_obj(c.F, val);
		}

		jit_insn_store(c.F, var, val);
	} else {
		jit_insn_store(c.F, var, VM::create_default(c.F, var_type));
	}
	return nullptr;
}

}
