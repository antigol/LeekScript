#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../value/Reference.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
	expression = nullptr;
	typeName = nullptr;
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

void VariableDeclaration::preanalyse(SemanticAnalyser* analyser)
{
	if (expression) {
		expression->preanalyse(analyser);

		if (typeName) {
			Type tn = typeName->getInternalType(analyser);

			// restrict expression types
			if (!Type::intersection(expression->type, tn, &expression->type)) {
				add_error(analyser, SemanticException::TYPE_MISMATCH);
			}
		}

		var_type = expression->type;
	} else {
		var_type = Type::UNKNOWN;
	}

	analyser->add_var(variable, var_type, expression, this);
	type = Type::VOID;
}

void VariableDeclaration::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	var_type.make_it_complete();

	if (expression) {
		expression->analyse(analyser, var_type);
	}

	analyser->add_var(variable, var_type, expression, this);
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
