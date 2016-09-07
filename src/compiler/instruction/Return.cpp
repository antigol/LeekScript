#include "Return.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

Return::Return() : Return(nullptr) {}

Return::Return(Value* v) {
	expression = v;
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

void Return::analyse_help(SemanticAnalyser* analyser)
{
	Function* f = analyser->current_function();

	if (expression) {
		expression->analyse(analyser);
		f->type.return_types.push_back(expression->type);
	} else {
		f->type.return_types.push_back(Type::VOID);
	}

	type = Type::UNREACHABLE;
}

void Return::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
}

void Return::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	Function* f = analyser->current_function();

	if (expression) {
		expression->finalize(analyser, f->type.return_type());
		f->type.set_return_type(expression->type);
	} else {
		if (!Type::intersection(Type::VOID, f->type.return_type())) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		f->type.set_return_type(Type::VOID);
	}

	type = Type::UNREACHABLE;
}

jit_value_t Return::compile(Compiler& c) const {

	if (expression) {
		jit_value_t v = expression->compile(c);

		if (expression->type.must_manage_memory()) {
			jit_value_t r = VM::move_obj(c.F, v);
			c.delete_variables_block(c.F, c.get_current_function_blocks());
			jit_insn_return(c.F, r);
		} else {
			c.delete_variables_block(c.F, c.get_current_function_blocks());
			jit_insn_return(c.F, v);
		}
	} else {
		jit_insn_return(c.F, nullptr);
	}

	return nullptr;
}

}
