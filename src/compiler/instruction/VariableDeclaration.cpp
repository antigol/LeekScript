#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../value/Reference.hpp"
#include "../jit/jit_general.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	expression = nullptr;
	typeName = nullptr;
	var = nullptr;
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
	if (debug && var) {
		os << " " << var->type;
	}
	if (expression) {
		os << " = ";
		expression->print(os, indent, debug);
	}
}

unsigned VariableDeclaration::line() const
{
	return 0;
}

// DONE 2
void VariableDeclaration::analyse_help(SemanticAnalyser* analyser)
{
	Type var_type = Type::UNKNOWN;

	if (typeName) {
		var_type = typeName->getInternalType();
		if (var_type == Type::UNKNOWN) add_error(analyser, SemanticException::UNKNOWN_TYPE);
	}
	if (expression) {
		Function* f = dynamic_cast<Function*>(expression);
		if (f != nullptr) {
			f->self_name = variable->content;
		}
		expression->analyse(analyser);
		if (!Type::intersection(var_type, expression->type, &var_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}

	var = analyser->add_var(variable->content, var_type, analyser->current_block());
	type = Type::VOID;

	if (var->type == Type::UNREACHABLE) {
		type = Type::UNREACHABLE;
	}
}

void VariableDeclaration::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (expression) {
		expression->reanalyse(analyser, var->type);
		if (!Type::intersection(var->type, expression->type, &var->type)) {
			add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
		}
	}

	if (var->type == Type::UNREACHABLE) {
		type = Type::UNREACHABLE;
	}
}

void VariableDeclaration::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (expression) {
		expression->finalize(analyser, var->type);
		var->type = expression->type;
	} else {
		var->type.make_it_pure();
	}

	if (var->type == Type::VOID) {
		add_error(analyser, SemanticException::CANT_ASSIGN_VOID);
	}
	if (var->type == Type::UNREACHABLE) {
		type = Type::UNREACHABLE;
	}
}

jit_value_t VariableDeclaration::compile(Compiler& c) const
{
	if (expression && expression->type == Type::UNREACHABLE) {
		return expression->compile(c);
	}

	jit_value_t v = jit_value_create(c.F, var->type.jit_type());

	if (expression) {
		jit_value_t val = expression->compile(c);

		val = jit_general::move_inc(c.F, val, expression->type);

		jit_insn_store(c.F, v, val);
	} else {
		jit_insn_store(c.F, v, jit_general::constant_default(c.F, var->type));
		jit_general::inc_refs(c.F, v, var->type);
	}

	c.add_var(variable->content, v, var->type, false);
	return nullptr;
}

}
