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

// DONE 2
void PostfixExpression::analyse_help(SemanticAnalyser* analyser)
{
	expression->analyse(analyser);

	if (!expression->isLeftValue()) {
		add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
	}
	if (!Type::intersection(expression->left_type, Type::ARITHMETIC, &expression->left_type)) {
		add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
	}

	type = expression->left_type;
}

void PostfixExpression::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	expression->reanalyse_l(analyser, Type::UNKNOWN, type);
	type = expression->left_type;
}

void PostfixExpression::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	expression->finalize_l(analyser, Type::UNKNOWN, type);
	type = expression->left_type;
}

jit_value_t PostfixExpression::compile(Compiler& c) const
{
	jit_value_t ptr = expression->compile_l(c);
	jit_value_t val = jit_insn_load_relative(c.F, ptr, 0, expression->left_type.jit_type());

	switch (operatorr->type) {
		case TokenType::PLUS_PLUS: {
			if (expression->left_type.raw_type->nature() == Nature::VALUE) {
				jit_value_t old = jit_insn_load(c.F, val);
				jit_value_t res = jit_insn_add(c.F, val, VM::create_i32(c.F, 1));
				jit_insn_store_relative(c.F, ptr, 0, res);
				return old;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_postinc, { val });
			}
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t old = jit_insn_load(c.F, val);
				jit_value_t res = jit_insn_sub(c.F, val, VM::create_i32(c.F, 1));
				jit_insn_store_relative(c.F, ptr, 0, res);
				return old;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_postdec, { val });
			}
		}
		default: break;
	}

	assert(0);
	return nullptr;
}

}
