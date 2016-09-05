#include "VariableDeclaration.hpp"
#include "../../vm/LSValue.hpp"
#include "../value/Reference.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
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
	os << " = ";
	expression->print(os, indent, debug);
}

unsigned VariableDeclaration::line() const
{
	return 0;
}

void VariableDeclaration::preanalyse(SemanticAnalyser* analyser)
{
	expression->preanalyse(analyser);

	if (typeName) {
		Type tn = typeName->getInternalType(analyser);

		// restrict expression types
		if (!Type::intersection(expression->type, tn, &expression->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}

	analyser->add_var(variable, expression->type, expression, this);
	type = Type::VOID;
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	expression->analyse(analyser, Type::UNKNOWN);
	analyser->add_var(variable, expression->type, expression, this);
}

jit_value_t VariableDeclaration::compile(Compiler& c) const {

	if (Reference* ref = dynamic_cast<Reference*>(expression)) {
		jit_value_t val = c.get_var(ref->variable->content).value;
		c.add_var(variable->content, val, expression->type, true);
	} else {
		jit_value_t var = jit_value_create(c.F, expression->type.jit_type());
		jit_value_t val = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			val = VM::move_inc_obj(c.F, val);
		}

		c.add_var(variable->content, var, expression->type, false);
		jit_insn_store(c.F, var, val);
	}
	return nullptr;
}

}
