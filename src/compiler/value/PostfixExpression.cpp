#include "PostfixExpression.hpp"
#include "../../vm/value/LSVar.hpp"
#include "../jit/jit_general.hpp"

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

	type = expression->left_type.image_conversion();
}

void PostfixExpression::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	expression->reanalyse_l(analyser, Type::UNKNOWN, type.fiber_conversion());
	if (!Type::intersection(type, expression->left_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void PostfixExpression::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	expression->finalize_l(analyser, Type::UNKNOWN, type.fiber_conversion());
	if (!Type::intersection(type, expression->left_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();
}

jit_value_t PostfixExpression::compile(Compiler& c) const
{
	jit_value_t ptr = expression->compile_l(c);
	jit_value_t val = jit_insn_load_relative(c.F, ptr, 0, expression->left_type.jit_type());
	jit_value_t out = nullptr;

	switch (operatorr->type) {
		case TokenType::PLUS_PLUS: {
			if (expression->left_type == Type::VAR) {
				out = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_postinc, { val });
			} else {
				out = jit_insn_load(c.F, val);
				jit_value_t res = jit_insn_add(c.F, val, jit_general::constant_i32(c.F, 1));
				jit_insn_store_relative(c.F, ptr, 0, res);
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type == Type::VAR) {
				out = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_postdec, { val });
			} else {
				out = jit_insn_load(c.F, val);
				jit_value_t res = jit_insn_sub(c.F, val, jit_general::constant_i32(c.F, 1));
				jit_insn_store_relative(c.F, ptr, 0, res);
			}
			break;
		}
		default: {
			assert(0);
		}
	}

	return jit_general::convert(c.F, out, expression->left_type, type);
}

}
