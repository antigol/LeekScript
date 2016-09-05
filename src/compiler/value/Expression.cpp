#include "../../compiler/value/Expression.hpp"

#include "../../compiler/value/IndexAccess.hpp"
#include "../../compiler/value/ObjectAccess.hpp"
#include "../../compiler/value/Function.hpp"
#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../compiler/value/VariableValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

Expression::Expression() : Expression(nullptr) {}

Expression::Expression(Value* v) :
	v1(v), v2(nullptr), op(nullptr), store_result_in_v1(false), no_op(false), operations(1) {
}

Expression::~Expression() {
	if (v1 != nullptr) {
		delete v1;
	}
	if (op != nullptr) {
		delete op;
	}
	if (v2 != nullptr) {
		delete v2;
	}
}

void Expression::append(Operator* op, Value* exp) {

	/*
	 * Single expression (2, 'hello', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2 = exp;
		return;
	}

	if (!parenthesis and (op->priority < this->op->priority
		   or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		Expression* ex = new Expression();
		ex->v1 = v2;
		ex->op = op;
		ex->v2 = exp;
		this->v2 = ex;

	} else {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a lower priority,
		 * such as : '< 7' => '(5 + 2) < 7'
		 */
		Expression* newV1 = new Expression();
		newV1->v1 = this->v1;
		newV1->op = this->op;
		newV1->v2 = this->v2;
		this->v1 = newV1;
		this->op = op;
		v2 = exp;
	}
}

void Expression::print(std::ostream& os, int indent, bool debug) const {

	if (parenthesis || debug) {
		os << "(";
	}

	if (v1 != nullptr) {

		v1->print(os, indent, debug);

		if (op != nullptr) {
			os << " ";
			op->print(os);
			os << " ";
			v2->print(os, indent, debug);
		}
	}
	if (parenthesis || debug) {
		os << ")";
	}
	if (debug) {
		os << " " << type;
	}
}

unsigned Expression::line() const {
	return 0;
}

void Expression::preanalyse(SemanticAnalyser* analyser)
{
	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->preanalyse(analyser);
		type = v1->type;
		return;
	}

	if (op->type == TokenType::EQUAL) {
		if (!v1->isLeftValue()) {
			v1->add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
		}

		v1->preanalyse(analyser);
		v2->preanalyse(analyser);

		Type result;
		if (!Type::intersection(((LeftValue*) v1)->left_type, v2->type, &result)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		v1->will_take(analyser, result);
		v2->will_require(analyser, result);

		type = v1->type;

	} else if (op->type == TokenType::PLUS) {

		v1->preanalyse(analyser);
		v2->preanalyse(analyser);


		if (!Type::intersection(v1->type, Type::ARITHMETIC, &v1->type)) {
			v1->add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}
		if (!Type::intersection(v2->type, Type::ARITHMETIC, &v2->type)) {
			v2->add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}

		Type result;
		if (!Type::intersection(v1->type, v2->type, &result)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}

		v1->will_require(analyser, result);
		v2->will_require(analyser, result);

		type = result.image_conversion();
	}
}

void Expression::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser, req_type);
		type = v1->type;
		return;
	}

	if (op->type == TokenType::EQUAL) {

		v1->analyse(analyser, req_type);
		v2->analyse(analyser, Type::UNKNOWN);
		type = v1->type;

	} else if (op->type == TokenType::PLUS) {

		if (!Type::intersection(type, req_type, &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_complete();

		v1->analyse(analyser, Type::UNKNOWN);
		v2->analyse(analyser, v1->type);

		Type result = v1->type.image_conversion();
		if (!Type::intersection(result, type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}
}

void EX_store_lsptr(LSValue** dest, LSValue* value) {
	LSValue::delete_ref(*dest);
	*dest = LSValue::move_inc(value);
}

jit_value_t Expression::compile(Compiler& c) const
{
	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	switch (op->type) {
		case TokenType::EQUAL: {
			// type = v1.type = v2.type
			LeftValue* left = (LeftValue*) v1;
			jit_value_t l = left->compile_l(c);
			jit_label_t label_end = jit_label_undefined;
			jit_insn_branch_if_not(c.F, l, &label_end);
			jit_value_t v = v2->compile(c);
			if (left->type.must_manage_memory()) {
				Compiler::call_native(c.F, LS_VOID, { LS_POINTER, LS_POINTER }, (void*) EX_store_lsptr, { l, v }); // TODO update for tuples
			} else {
				jit_insn_store_relative(c.F, l, 0, v);
			}
			jit_insn_label(c.F, &label_end);
			return left->compile(c);
		}

		case TokenType::PLUS: {
			// type = req_type
			// v1.type = v2.type
			jit_value_t x = v1->compile(c);
			jit_value_t y = v2->compile(c);
			if (v1->type.raw_type->nature() == Nature::LSVALUE) {
				jit_type_t args_t[2] = { LS_POINTER, LS_POINTER };
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_t, 2, 0);
				jit_value_t args[2] = { x, y };
				return jit_insn_call_native(c.F, "add", (void*) LSVar::ls_add, sig, args, 2, JIT_CALL_NOTHROW);
			} else {
				return Compiler::compile_convert(c.F, jit_insn_add(c.F, x, y), v1->type, type);
			}
		}

		default: break;
	}

	assert(0);
	return nullptr;
}

}

