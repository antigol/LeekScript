#include "../../compiler/instruction/VariableDeclaration.hpp"

#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"
#include "../value/Reference.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
}

VariableDeclaration::~VariableDeclaration() {
	delete expression;
	delete typeName;
}

void VariableDeclaration::print(ostream& os, int indent, bool debug) const {

	os << (global ? "global " : "let ");
	os << variable->content << " : ";
	typeName->print(os);
	os << " = ";
	expression->print(os, indent, debug);
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	if (typeName) {
		type = typeName->getInternalType(analyser);
		expression->analyse(analyser, type);
		if (expression->type != type) {
			analyser->add_error({SemanticException::Type::TYPE_MISMATCH, 0});
		}
	} else {
		expression->analyse(analyser, Type::UNKNOWN);
		type = expression->type;
	}

	analyser->add_var(variable, type, expression, this);
}

jit_value_t VariableDeclaration::compile(Compiler& c) const {

	if (Reference* ref = dynamic_cast<Reference*>(expression)) {
		jit_value_t val = c.get_var(ref->variable->content).value;
		c.add_var(variable->content, val, type, true);
		return val;
	} else {
		jit_value_t var = jit_value_create(c.F, VM::get_jit_type(type));
		jit_value_t val = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			val = VM::move_inc_obj(c.F, val);
		}

		c.add_var(variable->content, var, expression->type, false);
		jit_insn_store(c.F, var, val);
		return var;
	}
}

}
