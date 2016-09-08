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

// DONE 1
void PostfixExpression::analyse_help(SemanticAnalyser* analyser)
{
	expression->analyse(analyser);
	LeftValue* left = dynamic_cast<LeftValue*>(expression);

	if (!left->isLeftValue()) {
		add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
	}
	type = expression->left_type;
	if (!Type::intersection(type, Type::ARITHMETIC, &type)) {
		add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
	}
}

void PostfixExpression::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	LeftValue* left = dynamic_cast<LeftValue*>(expression);
	assert(left);
	left->reanalyse_l(analyser, Type::UNKNOWN, type);
	type = expression->left_type;
}

void PostfixExpression::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	LeftValue* left = dynamic_cast<LeftValue*>(expression);
	assert(left);
	left->finalize_l(analyser, Type::UNKNOWN, type);
	type = left->left_type;
}

jit_value_t PostfixExpression::compile(Compiler& c) const
{
	LeftValue* left = dynamic_cast<LeftValue*>(expression);
	assert(left);
	jit_value_t x = left->compile_l(c);
	jit_value_t y = jit_insn_load_relative(c.F, x, 0, left->left_type.jit_type());

	switch (operatorr->type) {
		case TokenType::PLUS_PLUS: {
			if (left->left_type.raw_type->nature() == Nature::VALUE) {
				jit_value_t old = jit_insn_load(c.F, y);
				jit_value_t sum = jit_insn_add(c.F, y, VM::create_i32(c.F, 1));
				jit_insn_store_relative(c.F, x, 0, sum);
				return old;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) &LSVar::ls_postinc, { y });
			}
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t old = jit_insn_load(c.F, y);
				jit_value_t sum = jit_insn_sub(c.F, y, VM::create_i32(c.F, 1));
				jit_insn_store_relative(c.F, x, 0, sum);
				return old;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) &LSVar::ls_postdec, { y });
			}
		}
		default: break;
	}

	assert(0);
	return nullptr;
}

}
