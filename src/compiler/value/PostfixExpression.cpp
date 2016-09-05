#include "PostfixExpression.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
}

PostfixExpression::~PostfixExpression() {
	delete expression;
	delete operatorr;
}

void PostfixExpression::print(ostream& os, int indent, bool debug) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

unsigned PostfixExpression::line() const {
	return 0;
}

void PostfixExpression::preanalyse(SemanticAnalyser* analyser)
{
	expression->preanalyse(analyser);
	if (!Type::intersection(expression->type, Type::ARITHMETIC, &expression->type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type = expression->type;
}

void PostfixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();

	expression->analyse(analyser, type);
}

jit_value_t PostfixExpression::compile(Compiler& c) const {

	jit_value_t x = expression->compile(c);

	switch (operatorr->type) {
		case TokenType::PLUS_PLUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = VM::create_i32(c.F, 1);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				return ox;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) &LSVar::ls_postinc, { x });
			}
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = VM::create_i32(c.F, 1);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				return ox;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) &LSVar::ls_postdec, { x });
			}
		}
		default: break;
	}

	assert(0);
	return nullptr;
}

}
