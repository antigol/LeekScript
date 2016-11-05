#include "../../compiler/instruction/Return.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return(Value* v) {
	expression = v;
	function = nullptr;
	in_function = false;
}

Return::~Return() {
	delete expression;
}

void Return::print(ostream& os, int indent, bool debug) const {
	os << "return ";
	expression->print(os, indent, debug);
}

void Return::analyse(SemanticAnalyser* analyser, const Type& ) {

	Function* f = analyser->current_function();
	if (f->type.getReturnType() == Type::UNKNOWN) {
		expression->analyse(analyser, Type::UNKNOWN);

		f->type.setReturnType(Type::UNKNOWN); // ensure that the vector is not empty
		f->type.return_types.push_back(expression->type);
	} else {
		expression->analyse(analyser, f->type.getReturnType());
	}
	function = f;
	in_function = true;

	type = Type::VOID;
}

jit_value_t Return::compile(Compiler& c) const {

	jit_value_t v = expression->compile(c);

	if (expression->type.must_manage_memory()) {
		jit_value_t r = VM::move_obj(c.F, v);
		c.delete_variables_block(c.F, c.get_current_function_blocks());
		jit_insn_return(c.F, r);
	} else {
		c.delete_variables_block(c.F, c.get_current_function_blocks());
		jit_insn_return(c.F, v);
	}

	return nullptr;
}

}
