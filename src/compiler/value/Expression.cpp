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

void Expression::analyse_help(SemanticAnalyser* analyser)
{
	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser);
		type = v1->type;
		return;
	}

	if (op->type == TokenType::EQUAL) {

		if (v1->isLeftValue()) {
			v1->analyse(analyser);
			v2->analyse(analyser);
			type = v1->type;
		} else {
			v1->add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
		}


	} else if (op->type == TokenType::PLUS
			   || op->type == TokenType::MINUS
			   || op->type == TokenType::TIMES
			   || op->type == TokenType::DIVIDE) {

		v1->analyse(analyser);
		v2->analyse(analyser);
		type = Type::UNKNOWN;

	} else if (op->type == TokenType::DOUBLE_EQUAL
			   || op->type == TokenType::DIFFERENT
			   || op->type == TokenType::LOWER
			   || op->type == TokenType::GREATER
			   || op->type == TokenType::LOWER_EQUALS
			   || op->type == TokenType::GREATER_EQUALS) {

		v1->analyse(analyser);
		v2->analyse(analyser);
		type = Type::BOOLEAN;

	} else if (op->type == TokenType::AND
			   || op->type == TokenType::OR) {

		v1->analyse(analyser);
		v2->analyse(analyser);
		type = Type::BOOLEAN;

	} else {
		assert(0);
	}
}

void Expression::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	DBOUT(cout << "EX wr type=" << type << " req=" << req_type << endl);
	if (op == nullptr) {
		v1->reanalyse(analyser, req_type);
		type = v1->type;
		return;
	}


	if (op->type == TokenType::EQUAL) {
		LeftValue* l1 = dynamic_cast<LeftValue*>(v1);

		Type old_left, old_right;
		do {
			old_left = l1->left_type;
			old_right = v2->type;
			l1->reanalyse_l(analyser, req_type, v2->type);
			type = l1->type;
			v2->reanalyse(analyser, l1->left_type);
		} while (analyser->errors.empty() && (old_left != l1->left_type || old_right != v2->type));

	} else if (op->type == TokenType::PLUS
			   || op->type == TokenType::MINUS
			   || op->type == TokenType::TIMES
			   || op->type == TokenType::DIVIDE) {
		if (!Type::intersection(type, req_type, &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		Type fiber = type.fiber_conversion();
		v1->reanalyse(analyser, fiber); // (a...) tricky : v1->type might be updated from fonction argument modification
		v2->reanalyse(analyser, fiber);

		Type type1, type2;
		if (!Type::intersection(v1->type, Type::ARITHMETIC, &type1)) {
			v1->add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}
		if (!Type::intersection(v2->type, Type::ARITHMETIC, &type2)) {
			v2->add_error(analyser, SemanticException::MUST_BE_ARITHMETIC_TYPE);
		}

		Type result;
		if (!Type::intersection(type1, type2, &result)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}

		v1->reanalyse(analyser, result);
		v2->reanalyse(analyser, result);

		// (...z) therefore this instruction is useful
		if (!Type::intersection(type, result.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		// TODO do {} while() here too ?
	} else if (op->type == TokenType::DOUBLE_EQUAL
			   || op->type == TokenType::DIFFERENT
			   || op->type == TokenType::LOWER
			   || op->type == TokenType::GREATER
			   || op->type == TokenType::LOWER_EQUALS
			   || op->type == TokenType::GREATER_EQUALS) {
		if (!Type::intersection(type, req_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		do {
			v1->reanalyse(analyser, v2->type);
			v2->reanalyse(analyser, v1->type);
		} while (analyser->errors.empty() && v1->type != v2->type);

	} else if (op->type == TokenType::AND
			   || op->type == TokenType::OR) {
		if (!Type::intersection(type, req_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		v1->reanalyse(analyser, Type::LOGIC);
		v2->reanalyse(analyser, Type::LOGIC);

	} else {
		assert(0);
	}

}

void Expression::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->finalize(analyser, req_type);
		type = v1->type;
		return;
	}

	if (op->type == TokenType::EQUAL) {
		LeftValue* l1 = dynamic_cast<LeftValue*>(v1);

		l1->finalize_l(analyser, req_type, v2->type);
		type = l1->type;
		v2->finalize(analyser, l1->left_type);

	} else if (op->type == TokenType::PLUS
			   || op->type == TokenType::MINUS
			   || op->type == TokenType::TIMES
			   || op->type == TokenType::DIVIDE) {

		v1->finalize(analyser, req_type.fiber_conversion());
		v2->finalize(analyser, v1->type);

		if (!Type::intersection(type, v1->type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();

	} else if (op->type == TokenType::DOUBLE_EQUAL
			   || op->type == TokenType::DIFFERENT
			   || op->type == TokenType::LOWER
			   || op->type == TokenType::GREATER
			   || op->type == TokenType::LOWER_EQUALS
			   || op->type == TokenType::GREATER_EQUALS) {
		if (!Type::intersection(type, req_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		v1->finalize(analyser, v2->type);
		v2->finalize(analyser, v1->type);

	} else if (op->type == TokenType::AND
			   || op->type == TokenType::OR) {
		if (!Type::intersection(type, req_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		v1->finalize(analyser, Type::LOGIC);
		v2->finalize(analyser, Type::LOGIC);

	} else {
		assert(0);
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

	if (op->type == TokenType::EQUAL) {
		// type = v1.type
		// v1.left_type = v2.type
		LeftValue* l1 = dynamic_cast<LeftValue*>(v1);
		jit_value_t l = l1->compile_l(c);
		jit_value_t v = v2->compile(c);
		if (l1->left_type.must_manage_memory()) {
			Compiler::call_native(c.F, LS_VOID, { LS_POINTER, LS_POINTER }, (void*) EX_store_lsptr, { l, v }); // TODO update for tuples
		} else {
			jit_insn_store_relative(c.F, l, 0, v);
		}
		return l1->compile(c);

	} else if (op->type == TokenType::PLUS
			   || op->type == TokenType::MINUS
			   || op->type == TokenType::TIMES
			   || op->type == TokenType::DIVIDE) {

		// v1.type => type
		// v1.type = v2.type
		jit_value_t x = v1->compile(c);
		jit_value_t y = v2->compile(c);

		switch (op->type) {
			case TokenType::PLUS: {

				if (v1->type.raw_type->nature() == Nature::LSVALUE) {
					return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_POINTER }, (void*) LSVar::ls_add, { x, y });
				} else {
					return Compiler::compile_convert(c.F, jit_insn_add(c.F, x, y), v1->type, type);
				}
			}
			case TokenType::MINUS: {
				if (v1->type.raw_type->nature() == Nature::LSVALUE) {
					return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_POINTER }, (void*) LSVar::ls_sub, { x, y });
				} else {
					return Compiler::compile_convert(c.F, jit_insn_sub(c.F, x, y), v1->type, type);
				}
			}
			case TokenType::TIMES: {
				if (v1->type.raw_type->nature() == Nature::LSVALUE) {
					return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_POINTER }, (void*) LSVar::ls_mul, { x, y });
				} else {
					return Compiler::compile_convert(c.F, jit_insn_mul(c.F, x, y), v1->type, type);
				}
			}
			case TokenType::DIVIDE: {
				if (v1->type.raw_type->nature() == Nature::LSVALUE) {
					return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_POINTER }, (void*) LSVar::ls_div, { x, y });
				} else {
					return Compiler::compile_convert(c.F, jit_insn_div(c.F, x, y), v1->type, type);
				}
			}
			default: {
				assert(0);
				return nullptr;
			}
		}
	} else if (op->type == TokenType::DOUBLE_EQUAL
			   || op->type == TokenType::DIFFERENT
			   || op->type == TokenType::LOWER
			   || op->type == TokenType::GREATER
			   || op->type == TokenType::LOWER_EQUALS
			   || op->type == TokenType::GREATER_EQUALS) {
		// v1.type = v2.type
		// type = boolean
		jit_value_t x = v1->compile(c);
		jit_value_t y = v2->compile(c);
		jit_value_t res;

		switch (op->type) {
			case TokenType::DOUBLE_EQUAL:   res = Compiler::compile_eq(c.F, x, v1->type, y, v2->type); break;
			case TokenType::DIFFERENT:      res = Compiler::compile_ne(c.F, x, v1->type, y, v2->type); break;
			case TokenType::LOWER:          res = Compiler::compile_lt(c.F, x, v1->type, y, v2->type); break;
			case TokenType::LOWER_EQUALS:   res = Compiler::compile_le(c.F, x, v1->type, y, v2->type); break;
			case TokenType::GREATER:        res = Compiler::compile_gt(c.F, x, v1->type, y, v2->type); break;
			case TokenType::GREATER_EQUALS: res = Compiler::compile_ge(c.F, x, v1->type, y, v2->type); break;
			default: {
				assert(0);
				return nullptr;
			}
		}

		if (v1->type.must_manage_memory()) VM::delete_temporary(c.F, x);
		if (v2->type.must_manage_memory()) VM::delete_temporary(c.F, y);

		return res;

	} else if (op->type == TokenType::AND
			   || op->type == TokenType::OR) {
		// v1.type != v2.type
		// type = boolean
		jit_value_t x = Compiler::compile_is_true_delete_temporary(c.F, v1->compile(c), v1->type);
		jit_label_t shortcut = jit_label_undefined;
		jit_value_t res = jit_value_create(c.F, LS_I32);

		switch (op->type) {
			case TokenType::AND: {
				jit_insn_store(c.F, res, VM::create_bool(c.F, false));
				jit_insn_branch_if_not(c.F, x, &shortcut);
				jit_value_t y = Compiler::compile_is_true_delete_temporary(c.F, v2->compile(c), v2->type);
				jit_insn_store(c.F, res, y);
				break;
			}
			case TokenType::OR: {
				jit_insn_store(c.F, res, VM::create_bool(c.F, true));
				jit_insn_branch_if(c.F, x, &shortcut);
				jit_value_t y = Compiler::compile_is_true_delete_temporary(c.F, v2->compile(c), v2->type);
				jit_insn_store(c.F, res, y);
				break;
			}
			default: {
				assert(0);
				return nullptr;
			}
		}
		jit_insn_label(c.F, &shortcut);
		return res;
	}

	assert(0);
	return nullptr;
}

}

