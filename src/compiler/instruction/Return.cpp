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

void Return::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	Function* f = analyser->current_function();

	if (expression) {
		if (f->type.return_type() == Type::UNKNOWN) { // Actually in PREanalyse
			expression->preanalyse(analyser);

			f->type.set_return_type(Type::UNKNOWN); // ensure that the vector is not empty
			f->type.return_types.push_back(expression->type);
		} else {
			expression->analyse(analyser, f->type.return_type());
			if (expression->type != f->type.return_type()) {
				analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
			}
		}
	} else {
		if (f->type.return_type() == Type::UNKNOWN) {
			f->type.set_return_type(Type::UNKNOWN);
			f->type.return_types.push_back(Type::VOID);
		} else {
			if (f->type.return_type() != Type::VOID) {
				analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
			}
		}
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
