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

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (typeName) {
		Type tn = typeName->getInternalType(analyser);
		expression->analyse(analyser, tn);
		if (expression->type != tn) {
			analyser->add_error({SemanticException::Type::TYPE_MISMATCH, 0});
		}
	} else {
		expression->analyse(analyser, Type::UNKNOWN);
	}

	analyser->add_var(variable, expression->type, expression, this);

	// return type

	if (req_type == Type::VOID) {
		type = Type::VOID;
	} else if (req_type == Type::UNKNOWN) {
		type = expression->type;
	} else if (expression->type.can_be_convert_in(req_type)) {
		type = req_type;
	} else {
		analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
	}
	assert(type.is_complete());
}

jit_value_t VariableDeclaration::compile(Compiler& c) const {

	if (Reference* ref = dynamic_cast<Reference*>(expression)) {
		jit_value_t val = c.get_var(ref->variable->content).value;
		c.add_var(variable->content, val, expression->type, true);
		if (type != Type::VOID) return Compiler::compile_convert(c.F, val, expression->type, type);
	} else {
		jit_value_t var = jit_value_create(c.F, VM::get_jit_type(expression->type));
		jit_value_t val = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			val = VM::move_inc_obj(c.F, val);
		}

		c.add_var(variable->content, var, expression->type, false);
		jit_insn_store(c.F, var, val);
		if (type != Type::VOID) return Compiler::compile_convert(c.F, var, expression->type, type);
	}
}

}
