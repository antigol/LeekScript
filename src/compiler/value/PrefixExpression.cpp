#include "PrefixExpression.hpp"
#include "LeftValue.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSVar.hpp"
#include "../../vm/LSValue.hpp"
#include "../jit/jit_general.hpp"

using namespace std;

namespace ls {

PrefixExpression::PrefixExpression() {
	expression = nullptr;
	operatorr = nullptr;
}

PrefixExpression::~PrefixExpression() {
	delete expression;
	delete operatorr;
}

void PrefixExpression::print(ostream& os, int indent, bool debug) const {
	operatorr->print(os);
	if (operatorr->type == TokenType::NEW) {
		os << " ";
	}
	expression->print(os, indent, debug);
	if (debug) {
		os << " " << type;
	}
}

unsigned PrefixExpression::line() const {
	return 0;
}

// DONE 2
void PrefixExpression::analyse_help(SemanticAnalyser* analyser)
{
	expression->analyse(analyser);

	if (operatorr->type == TokenType::PLUS_PLUS
			|| operatorr->type == TokenType::MINUS_MINUS) {

		LeftValue* left = dynamic_cast<LeftValue*>(expression);

		if (!left || !left->isLeftValue()) {
			add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
			return;
		}
		if (!Type::intersection(left->left_type, Type::ARITHMETIC, &left->left_type)) {
			add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}

		type = left->left_type.image_conversion();
	} else if (operatorr->type == TokenType::MINUS
			   || operatorr->type == TokenType::TILDE) {

		if (!Type::intersection(expression->type, Type::ARITHMETIC, &expression->type)) {
			add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}
		type = expression->type.image_conversion();
	} else if (operatorr->type == TokenType::NOT) {
		if (!Type::intersection(expression->type, Type::LOGIC, &expression->type)) {
			add_error(analyser, SemanticException::MUST_BE_LOGIC_TYPE);
		}
		type = Type::BOOLEAN.image_conversion();
	} else {
		assert(0);
	}
}

void PrefixExpression::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (operatorr->type == TokenType::PLUS_PLUS
			|| operatorr->type == TokenType::MINUS_MINUS) {

		LeftValue* left = dynamic_cast<LeftValue*>(expression);
		left->reanalyse_l(analyser, Type::UNKNOWN, type.fiber_conversion());
		if (!Type::intersection(type, left->left_type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

	} else if (operatorr->type == TokenType::MINUS
			   || operatorr->type == TokenType::TILDE) {

		expression->reanalyse(analyser, type.fiber_conversion());
		if (!Type::intersection(type, expression->type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

	} else if (operatorr->type == TokenType::NOT) {
		expression->reanalyse(analyser, Type::UNKNOWN);
	}

}

void PrefixExpression::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (operatorr->type == TokenType::PLUS_PLUS
			|| operatorr->type == TokenType::MINUS_MINUS) {

		LeftValue* left = dynamic_cast<LeftValue*>(expression);
		left->finalize_l(analyser, Type::UNKNOWN, type.fiber_conversion());
		if (!Type::intersection(type, left->left_type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();

	} else if (operatorr->type == TokenType::MINUS
			   || operatorr->type == TokenType::TILDE) {

		expression->finalize(analyser, type.fiber_conversion());
		if (!Type::intersection(type, expression->type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();

	} else if (operatorr->type == TokenType::NOT) {
		expression->finalize(analyser, Type::UNKNOWN);
		type.make_it_pure();
	}
}

int32_t PE_not(LSValue* value) {
	if (value == nullptr) return true;
	int32_t r = !value->isTrue();
	if (value->refs == 0) delete value;
	return r;
}

jit_value_t PrefixExpression::compile(Compiler& c) const
{
	if (operatorr->type == TokenType::PLUS_PLUS
			|| operatorr->type == TokenType::MINUS_MINUS) {

		LeftValue* left = dynamic_cast<LeftValue*>(expression);

		jit_value_t ptr = left->compile_l(c);
		jit_value_t val = jit_insn_load_relative(c.F, ptr, 0, left->left_type.jit_type());
		jit_value_t res = nullptr;

		switch (operatorr->type) {
			case TokenType::PLUS_PLUS: {
				if (left->left_type.raw_type->nature() == Nature::VALUE) {
					res = jit_insn_add(c.F, val, jit_general::constant_i32(c.F, 1));
					jit_insn_store_relative(c.F, ptr, 0, res);
				} else {
					res = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_preinc, { val });
				}
				break;
			}
			case TokenType::MINUS_MINUS: {
				if (left->type.raw_type->nature() == Nature::VALUE) {
					res = jit_insn_sub(c.F, val, jit_general::constant_i32(c.F, 1));
					jit_insn_store_relative(c.F, ptr, 0, res);
				} else {
					res = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_predec, { val });
				}
				break;
			}
			default: break;
		}
		return jit_general::convert(c.F, res, left->left_type, type);

	} else if (operatorr->type == TokenType::MINUS
			   || operatorr->type == TokenType::TILDE) {

		jit_value_t val = expression->compile(c);
		jit_value_t res = nullptr;

		switch (operatorr->type) {
			case TokenType::MINUS: {
				if (expression->type.raw_type->nature() == Nature::VALUE) {
					res = jit_insn_neg(c.F, val);
				} else {
					res = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_minus, { val });
				}
				break;
			}
			case TokenType::TILDE: {
				if (expression->type.raw_type->nature() == Nature::VALUE) {
					res = jit_insn_not(c.F, val);
				} else {
					res = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_tilde, { val });
				}
				break;
			}
			default: break;
		}
		return jit_general::convert(c.F, res, expression->type, type);

	} else if (operatorr->type == TokenType::NOT) {
		jit_value_t val = expression->compile(c);
		jit_value_t res = nullptr;

		if (expression->type.raw_type->nature() == Nature::VALUE) {
			res = jit_insn_to_not_bool(c.F, val);
		} else {
			res = jit_general::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) PE_not, { val });
		}
		return jit_general::convert(c.F, res, Type::BOOLEAN, type);
	}

	assert(0);
	return nullptr;
}

}
