#include "../../compiler/value/PostfixExpression.hpp"

#include "../../compiler/value/LeftValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
	return_value = true;
}

PostfixExpression::~PostfixExpression() {
	delete expression;
}

void PostfixExpression::print(ostream& os, int indent, bool debug) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

Location PostfixExpression::location() const {
	return expression->location(); // TODO add the op
}

void PostfixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	expression->analyse(analyser, Type::UNKNOWN);

	if (expression->type.constant) {
		analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), expression->location(), {expression->to_string()}});
	}
	if (!expression->isLeftValue()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_LVALUE, location(), expression->location(), {expression->to_string()}});
	}

	type = expression->type;
	if (type == Type::MPZ) {
		type.temporary = true;
	}
	this->return_value = return_value;

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
}

Compiler::value PostfixExpression::compile(Compiler& c) const {

	c.inc_ops(1);

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {

			if (expression->type == Type::MPZ) {

				auto x = expression->compile(c);
				auto r = c.insn_clone_mpz(x);
				auto x_addr = c.insn_address_of(x);
				auto one = c.new_integer(1);
				c.insn_call(Type::VOID, {x_addr, x_addr, one}, &mpz_add_ui);
				return r;

			} else if (expression->type.nature == Nature::VALUE) {

				auto x_addr = expression->compile_l(c);

				jit_value_t x = jit_insn_load_relative(c.F, x_addr.v, 0, jit_type_int);

				auto sum = c.insn_add({x, Type::INTEGER}, c.new_integer(1));
				jit_insn_store_relative(c.F, x_addr.v, 0, sum.v);

				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, x, type), type};
				}
				return {x, type};
			} else {
				auto e = expression->compile_l(c);
				return c.insn_call(Type::POINTER, {e}, (void*) +[](LSValue** x) {
					return (*x)->ls_inc();
				});
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x.v);
				jit_value_t y = LS_CREATE_INTEGER(c.F, 1);
				jit_value_t sum = jit_insn_sub(c.F, x.v, y);
				jit_insn_store(c.F, x.v, sum);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, ox, type), type};
				}
				return {ox, type};
			} else {
				auto e = expression->compile_l(c);
				return c.insn_call(Type::POINTER, {e}, (void*) +[](LSValue** x) {
					return (*x)->ls_dec();
				});
			}
			break;
		}
		default: {}
	}
	return {nullptr, Type::UNKNOWN};
}

Value* PostfixExpression::clone() const {
	auto pe = new PostfixExpression();
	pe->expression = (LeftValue*) expression->clone();
	pe->operatorr = operatorr;
	return pe;
}

}
