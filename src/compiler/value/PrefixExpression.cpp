#include "PrefixExpression.hpp"
#include "FunctionCall.hpp"
#include "LeftValue.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSVar.hpp"
#include "../../vm/LSValue.hpp"

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

void PrefixExpression::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
}

void PrefixExpression::will_require(SemanticAnalyser* analyser, const Type& req_type)
{

}

void PrefixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
	if (operatorr->type == TokenType::PLUS_PLUS
		|| operatorr->type == TokenType::MINUS_MINUS
		|| operatorr->type == TokenType::MINUS
		|| operatorr->type == TokenType::TILDE) {

		expression->analyse(analyser, req_type);
//		if (!expression->type.is_arithmetic()) {
//			analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
//		}

		type = expression->type;

	} else if (operatorr->type == TokenType::NOT) {
		expression->analyse(analyser, Type::UNKNOWN);
		type = Type::BOOLEAN;

		if (req_type == Type::VAR) {
			type = Type::VAR;
		} else if (req_type != Type::UNKNOWN) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}
	assert(type.is_pure() || !analyser->errors.empty());
}

int32_t PE_not(LSValue* value) {
	if (value == nullptr) return true;
	int32_t r = !value->isTrue();
	if (value->refs == 0) delete value;
	return r;
}

jit_value_t PrefixExpression::compile(Compiler& c) const
{
	jit_value_t x = expression->compile(c);

	switch (operatorr->type) {
		case TokenType::PLUS_PLUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_insn_store(c.F, x, jit_insn_add(c.F, x, VM::create_i32(c.F, 1)));
				return x;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_preinc, { x });
			}
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				jit_insn_store(c.F, x, jit_insn_sub(c.F, x, VM::create_i32(c.F, 1)));
				return x;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_predec, { x });
			}
		}
		case TokenType::MINUS: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				return jit_insn_neg(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_minus, { x });
			}
		}
		case TokenType::TILDE: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				return jit_insn_not(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_minus, { x });
			}
		}
		case TokenType::NOT: {
			if (expression->type.raw_type->nature() == Nature::VALUE) {
				return jit_insn_to_not_bool(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) PE_not, { x });
			}
		}
		default: break;
	}

	assert(0);
	return nullptr;
}

}
