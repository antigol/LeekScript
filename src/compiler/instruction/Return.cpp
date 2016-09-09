#include "Return.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return() : Return(nullptr) {}

Return::Return(Value* v) {
	expression = v;
	function = nullptr;
}

Return::~Return() {
	delete expression;
}

void Return::print(ostream& os, int indent, bool debug) const {
	os << "return ";
	if (expression) expression->print(os, indent, debug);
}

unsigned Return::line() const
{
	return 0;
}

// DONE 1
void Return::analyse_help(SemanticAnalyser* analyser)
{
	function = analyser->current_function();

	Type tmp;
	if (expression) {
		expression->analyse(analyser);
		if (!Type::intersection(expression->type, function->type.return_type(), &tmp)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else {
		if (!Type::intersection(Type::VOID, function->type.return_type(), &tmp)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}
	if (!Type::intersection(function->type.return_types[0], tmp, &function->type.return_types[0])) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type = Type::UNREACHABLE;
}

void Return::reanalyse_help(SemanticAnalyser* analyser, const Type&)
{
	Type tmp;
	if (expression) {
		expression->reanalyse(analyser, function->type.return_type());
		if (!Type::intersection(expression->type, function->type.return_types[0], &function->type.return_types[0])) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else {
		if (!Type::intersection(Type::VOID, function->type.return_types[0], &function->type.return_types[0])) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}
	//tip! type = UNREACHABLE
}

void Return::finalize_help(SemanticAnalyser* analyser, const Type&)
{
	if (expression) {
		expression->finalize(analyser, function->type.return_type());
		function->type.set_return_type(expression->type);
	} else {
		if (!Type::intersection(Type::VOID, function->type.return_type())) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		function->type.set_return_type(Type::VOID);
	}
	//tip! type = UNREACHABLE
}

jit_value_t Return::compile(Compiler& c) const
{
	jit_value_t v = nullptr;

	if (expression) {
		v = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			v = VM::move_obj(c.F, v);
		}
	}

	c.delete_variables_block(c.F, c.get_current_function_blocks());

	// Delete temporary arguments
	for (size_t i = 0; i < function->type.arguments_types.size(); ++i) {
		if (function->type.argument_type(i).must_manage_memory()) {
			jit_value_t p = jit_value_get_param(c.F, i);
			VM::delete_ref(c.F, p);
		}
	}

	jit_insn_return(c.F, v);
	return nullptr;
}

}
