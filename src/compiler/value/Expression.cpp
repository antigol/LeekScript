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

void Expression::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser, req_type);
		type = v1->type;
		return;
	}

	if (op->type == TokenType::EQUAL) {
		v1->analyse(analyser, Type::UNKNOWN);
		if (!v1->isLeftValue()) {
			stringstream oss;
			v1->print(oss, 0, false);
			analyser->add_error({ SemanticException::VALUE_MUST_BE_A_LVALUE, v1->line(), oss.str() });
		}
		v2->analyse(analyser, v1->type);

		type = v1->type;
		if (!Type::get_intersection(type, req_type)) {
			stringstream oss;
			print(oss, 0, false);
			analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
		}

	} else if (op->type == TokenType::PLUS) {
		v1->preanalyse(analyser);
		if (!v1->type.is_arithmetic()) {
			stringstream oss;
			v1->print(oss, 0, false);
			analyser->add_error({ SemanticException::MUST_BE_ARITHMETIC_TYPE, v1->line(), oss.str() });
		}
		v2->preanalyse(analyser);
		if (!v2->type.is_arithmetic()) {
			stringstream oss;
			v2->print(oss, 0, false);
			analyser->add_error({ SemanticException::MUST_BE_ARITHMETIC_TYPE, v2->line(), oss.str() });
		}

		if (!Type::get_intersection(v1->type, v2->type, &type)) {
			stringstream oss;
			print(oss, 0, false);
			analyser->add_error({ SemanticException::INCOMPATIBLE_TYPES, line(), oss.str() });
		}
		type.make_it_complete(); // because we need v1 and v2 to have the same type
		v1->analyse(analyser, type);
		v2->analyse(analyser, type);

		if (!Type::get_intersection(type, req_type)) {
			if (type.can_be_convert_in(req_type)) {
				type = req_type;
				type.make_it_complete();
			} else {
				stringstream oss;
				print(oss, 0, false);
				analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
			}
		}
	}

	/*
	operations = 1;
	type = Type::VAR;

	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser, req_type);
		type = v1->type;
		return;
	}

	// Require float type for a division
	Type v1_type = Type::UNKNOWN;
	Type v2_type = Type::UNKNOWN;

	if (op->type == TokenType::DIVIDE) {
		type.raw_type = RawType::FLOAT;
		v1_type = Type::FLOAT;
		v2_type = Type::FLOAT;
	}

	// First analyse of v1 and v2
	v1->analyse(analyser, v1_type);
	if (v1->type.raw_type->nature() == Nature::LSVALUE) {
		type.raw_type->nature() = Nature::LSVALUE;
	}
	v2->analyse(analyser, v2_type);
	if (v2->type.raw_type->nature() == Nature::LSVALUE) {
		type.raw_type->nature() = Nature::LSVALUE;
	}
	constant = v1->constant and v2->constant;

	// A = B, A += B, A * B, etc. mix types
	if (op->type == TokenType::EQUAL or op->type == TokenType::XOR
		or op->type == TokenType::PLUS or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::TIMES or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE or op->type == TokenType::DIVIDE_EQUAL
		or op->type == TokenType::MINUS or op->type == TokenType::MINUS_EQUAL
		or op->type == TokenType::POWER or op->type == TokenType::POWER_EQUAL
		or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER or op->type == TokenType::GREATER_EQUALS
		or op->type == TokenType::SWAP) {

		// Set the correct type nature for the two members
		if (v2->type.raw_type->nature() == Nature::LSVALUE and v1->type.raw_type->nature() != Nature::LSVALUE) {
			v1->analyse(analyser, Type::POINTER);
		}
		if (v1->type.raw_type->nature() == Nature::LSVALUE and v2->type.raw_type->nature() != Nature::LSVALUE) {
			v2->analyse(analyser, Type::POINTER);
		}

		type = v1->type.mix(v2->type);

		// String / String => Array<String>
		if (op->type == TokenType::DIVIDE and v1->type == Type::STRING and v2->type == Type::STRING) {
			type = Type::STRING_ARRAY;
		}
	}

	// Boolean operators : result is a boolean
	if (op->type == TokenType::AND or op->type == TokenType::OR or op->type == TokenType::XOR
		or op->type == TokenType::GREATER or op->type == TokenType::DOUBLE_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER_EQUALS or op->type == TokenType::TRIPLE_EQUAL
		or op->type == TokenType::DIFFERENT or op->type == TokenType::TRIPLE_DIFFERENT) {

		type = Type::BOOLEAN;
	}

	// Bitwise operators : result is a integer
	if (op->type == TokenType::BIT_AND or op->type == TokenType::PIPE
		or op->type == TokenType::BIT_XOR or op->type == TokenType::BIT_AND_EQUALS
		or op->type == TokenType::BIT_OR_EQUALS or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS) {

		type = Type::INTEGER;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::POWER_EQUAL) {
		// TODO other operators like |= ^= &=

		// Check if A is a l-value
		bool is_left_value = true;
		if (!v1.isLeftValue()) {
			std::string c = "<v>";
			analyser->add_error({SemanticException::Type::VALUE_MUST_BE_A_LVALUE, v1->line(), c});
			is_left_value = false;
		}

		// A += B, A -= B
		if (is_left_value and (op->type == TokenType::PLUS_EQUAL or op->type == TokenType::MINUS_EQUAL)) {
			if (v1->type == Type::INTEGER and v2->type == Type::FLOAT) {
				((LeftValue*) v1)->change_type(analyser, Type::FLOAT);
			}
		}

		store_result_in_v1 = true;
	}

	// [1, 2, 3] ~~ x -> x ^ 2
	if (op->type == TokenType::TILDE_TILDE) {
		v2->will_take(analyser, { v1->type.getElementType(0) });
		type = Type::PTR_ARRAY;
		type.setElementType(v2->type.getReturnType());
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = Type::POINTER;
		if (v1->type == v2->type) type = v1->type;
	}

	if (op->type == TokenType::INSTANCEOF) {
		v1->analyse(analyser, Type::POINTER);
		type = Type::BOOLEAN;
	}
	if (op->type == TokenType::IN) {
		v1->analyse(analyser, Type::POINTER);
		type = Type::BOOLEAN;
	}

	// Merge operations count
	// (2 + 3) × 4    ->  2 ops for the × directly
	if (op->type != TokenType::OR or op->type == TokenType::AND) {
		if (Expression* ex1 = dynamic_cast<Expression*>(v1)) {
			operations += ex1->operations;
			ex1->operations = 0;
		}
		if (Expression* ex2 = dynamic_cast<Expression*>(v2)) {
			operations += ex2->operations;
			ex2->operations = 0;
		}
	}

	// At the end the require nature is taken into account
	if (req_type.raw_type->nature() != Nature::UNKNOWN) {
		type.raw_type->nature() = req_type.raw_type->nature();
	}
	*/
	assert(type.is_complete() || !analyser->errors.empty());
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
		v1->preanalyse(analyser);
		v2->preanalyse(analyser);
		type = v1->type;
	} else if (op->type == TokenType::PLUS) {
		v1->preanalyse(analyser);
		v2->preanalyse(analyser);
		Type::get_intersection(v1->type, v2->type, &type);
	}
}

/*
LSValue* jit_add(LSValue* x, LSValue* y) {
	return x->ls_add(y);
}
LSValue* jit_sub(LSValue* x, LSValue* y) {
	return x->ls_sub(y);
}
LSValue* jit_mul(LSValue* x, LSValue* y) {
	return x->ls_mul(y);
}
LSValue* jit_div(LSValue* x, LSValue* y) {
	return x->ls_div(y);
}
LSValue* jit_pow(LSValue* x, LSValue* y) {
	return x->ls_pow(y);
}
LSValue* jit_mod(LSValue* x, LSValue* y) {
	return x->ls_mod(y);
}
bool jit_and(LSValue* x, LSValue* y) {
	bool r = x->isTrue() and y->isTrue();
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_or(LSValue* x, LSValue* y) {
	bool r = (x->isTrue() or y->isTrue());
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_xor(LSValue* x, LSValue* y) {
	bool r = (x->isTrue() xor y->isTrue());
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

bool jit_equals(LSValue* x, LSValue* y) {
	bool r = *x == *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_not_equals(LSValue* x, LSValue* y) {
	bool r = *x != *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_lt(LSValue* x, LSValue* y) {
	bool r = *x < *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_le(LSValue* x, LSValue* y) {
	bool r = *x <= *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_gt(LSValue* x, LSValue* y) {
	bool r = *x > *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_ge(LSValue* x, LSValue* y) {
	bool r = *x >= *y;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

LSValue* jit_store(LSValue** x, LSValue* y) {
	y->refs++;
	LSValue* r = *x = y;
	LSValue::delete_ref(y);
	return r;
}

int jit_store_value(int* x, int y) {
	return *x = y;
}

LSValue* jit_swap(LSValue** x, LSValue** y) {
	LSValue* tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

LSValue* jit_add_equal(LSValue* x, LSValue* y) {
	return x->ls_add_eq(y);
}
int jit_add_equal_value(int* x, int y) {
	return *x += y;
}
LSValue* jit_sub_equal(LSValue* x, LSValue* y) {
	return x->ls_sub_eq(y);
}
LSValue* jit_mul_equal(LSValue* x, LSValue* y) {
	return x->ls_mul_eq(y);
}
LSValue* jit_div_equal(LSValue* x, LSValue* y) {
	return x->ls_div_eq(y);
}
LSValue* jit_mod_equal(LSValue* x, LSValue* y) {
	return x->ls_mod_eq(y);
}
LSValue* jit_pow_equal(LSValue* x, LSValue* y) {
	return x->ls_pow_eq(y);
}

bool jit_in(LSValue* x, LSValue* y) {
	bool r = y->in(x);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

bool jit_instanceof(LSValue* x, LSValue* y) {
	bool r = (((LSClass*) x->getClass())->name == ((LSClass*) y)->name);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}

LSValue* jit_bit_and(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_and_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_or(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_or_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_xor(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_xor_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shl(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shl_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shr(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shr_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shr_unsigned(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
LSValue* jit_bit_shr_unsigned_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return new LSVar();
}
bool jit_is_null(LSValue* v) {
	return v->typeID() == 1;
}
*/

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
				Compiler::call_native(c.F, LS_VOID, { LS_POINTER, LS_POINTER }, (void*) EX_store_lsptr, { l, v });
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
/*
	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	// Increment operations
	if (operations > 0) {
		VM::inc_ops(c.F, operations);
	}

//	cout << "v1 : " << v1->type << ", v2 : " << v2->type << endl;

	jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
	void* ls_func;
	bool use_jit_func = v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE;
	vector<jit_value_t> args;
	Type jit_returned_type = Type::UNKNOWN;
	Type ls_returned_type = Type::POINTER; // By default ls_ function returns pointers

	switch (op->type) {
		case TokenType::EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {

				if (IndexAccess* l1 = dynamic_cast<IndexAccess*>(v1)) {

					args.push_back(l1->compile_l(c));
					args.push_back(v2->compile(c));

					jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
					jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I32, args_types, 2, 0);
					jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_store_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

					if (type.raw_type->nature() == Nature::LSVALUE) {
						return VM::value_to_pointer(c.F, v, type);
					}
					return v;
				} else {

					jit_value_t x = v1->compile(c);
					jit_value_t y = v2->compile(c);
					jit_insn_store(c.F, x, y);
					if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
						return VM::value_to_pointer(c.F, y, type);
					}
					return y;
				}
			} else if (v1->type.raw_type->nature() == Nature::LSVALUE) {

				if (dynamic_cast<IndexAccess*>(v1)) {

					args.push_back(((LeftValue*) v1)->compile_l(c));
					args.push_back(v2->compile(c));

					if (v1->type.must_manage_memory()) {
						args[1] = VM::move_inc_obj(c.F, args[1]);
					}

					ls_func = (void*) &jit_store;

				} else if (dynamic_cast<VariableValue*>(v1)) {

					jit_value_t x = v1->compile(c);
					jit_value_t y = v2->compile(c);

					if (v2->type.must_manage_memory()) {
						y = VM::move_inc_obj(c.F, y);
					}
					if (v1->type.must_manage_memory()) {
						VM::delete_ref(c.F, x);
					}
					jit_insn_store(c.F, x, y);
					return y;

				} else {
					args.push_back(((LeftValue*) v1)->compile_l(c));
					args.push_back(v2->compile(c));
					ls_func = (void*) &jit_store;
				}
			} else {
				cout << "type : " << type << endl;
				cout << "v1: " << v1->type << endl;
				cout << "v2: " << v2->type << endl;
				throw new runtime_error("value = pointer !");
			}
			break;
		}
		case TokenType::SWAP: {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t t = jit_insn_load(c.F, x);
				jit_insn_store(c.F, x, y);
				jit_insn_store(c.F, y, t);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, x, type);
				}
				return x;
			} else {
				args.push_back(((LeftValue*) v1)->compile_l(c));
				args.push_back(((LeftValue*) v2)->compile_l(c));
				ls_func = (void*) &jit_swap;
			}
			break;
		}
		case TokenType::PLUS_EQUAL: {

			if (IndexAccess* l1 = dynamic_cast<IndexAccess*>(v1)) {

				args.push_back(l1->compile_l(c));
				args.push_back(v2->compile(c));

				jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I32, args_types, 2, 0);
				jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_add_equal_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

				if (type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, v, type);
				}
				return v;
			}

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_add_equal;
			}
			break;
		}
		case TokenType::MINUS_EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_sub_equal;
			}
			break;
		}
		case TokenType::TIMES_EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {

				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_mul(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mul_equal;
			}
			break;
		}
		case TokenType::DIVIDE_EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t xf = jit_value_create(c.F, LS_F64);
				jit_insn_store(c.F, xf, x);
				jit_value_t sum = jit_insn_div(c.F, xf, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_div_equal;
			}
			break;
		}
		case TokenType::MODULO_EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_rem(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_mod_equal;
			}
			break;
		}
		case TokenType::POWER_EQUAL: {

			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t sum = jit_insn_pow(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				ls_func = (void*) &jit_pow_equal;
			}
			break;
		}
		case TokenType::AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_and;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::OR: {
			jit_func = &jit_insn_or;
			ls_func = (void*) &jit_or;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::XOR: {
			if (use_jit_func) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				x = jit_insn_to_not_bool(c.F, x);
				y = jit_insn_to_not_bool(c.F, y);
				jit_value_t r = jit_insn_or(c.F,
					jit_insn_and(c.F, x, jit_insn_not(c.F, y)),
					jit_insn_and(c.F, y, jit_insn_not(c.F, x))
				);
				if (type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, r, Type::BOOLEAN);
				}
				return r;
			} else {
				ls_func = (void*) &jit_xor;
				ls_returned_type = Type::BOOLEAN;
			}
			break;
		}
		case TokenType::PLUS: {
			jit_func = &jit_insn_add;
			ls_func = (void*) &jit_add;
			break;
		}
		case TokenType::MINUS: {
			jit_func = &jit_insn_sub;
			ls_func = (void*) &jit_sub;
			break;
		}
		case TokenType::TIMES: {
			jit_func = &jit_insn_mul;
			ls_func = (void*) &jit_mul;
			break;
		}
		case TokenType::DIVIDE: {
			jit_func = &jit_insn_div;
			ls_func = (void*) &jit_div;
			jit_returned_type = Type::FLOAT;
			break;
		}
		case TokenType::MODULO: {
			jit_func = &jit_insn_rem;
			ls_func = (void*) &jit_mod;
			break;
		}
		case TokenType::POWER: {
			jit_func = &jit_insn_pow;
			ls_func = (void*) &jit_pow;
			break;
		}
		case TokenType::DOUBLE_EQUAL: {
			jit_func = &jit_insn_eq;
			ls_func = (void*) &jit_equals;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::DIFFERENT: {
			jit_func = &jit_insn_ne;
			ls_func = (void*) &jit_not_equals;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER: {
			if (use_jit_func) {
				if (v1->type == Type::BOOLEAN && v2->type.isNumber()) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, true);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(true));
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, false);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(false));
				}
			}
			jit_func = &jit_insn_lt;
			ls_func = (void*) &jit_lt;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::LOWER_EQUALS: {
			if (use_jit_func) {
				if (v1->type == Type::BOOLEAN && v2->type.isNumber()) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, true);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(true));
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, false);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(false));
				}
			}
			jit_func = &jit_insn_le;
			ls_func = (void*) &jit_le;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER: {
			if (use_jit_func) {
				if (v1->type == Type::BOOLEAN && v2->type.isNumber()) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, false);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(false));
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, true);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(true));
				}
			}
			jit_func = &jit_insn_gt;
			ls_func = (void*) &jit_gt;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::GREATER_EQUALS: {
			if (use_jit_func) {
				if (v1->type == Type::BOOLEAN && v2->type.isNumber()) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, false);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(false));
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.raw_type->nature() == Nature::VALUE) return LS_CREATE_BOOLEAN(c.F, true);
					else return LS_CREATE_POINTER(c.F, LSBoolean::get(true));
				}
			}
			jit_func = &jit_insn_ge;
			ls_func = (void*) &jit_ge;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
		case TokenType::TILDE_TILDE: {
			if (v1->type.getElementType(0) == Type::INTEGER) {
				if (type.getElementType(0) == Type::INTEGER) {
					ls_func = (void*) &LSVec<int>::ls_map_int;
				} else if (type.getElementType(0) == Type::FLOAT) {
					ls_func = (void*) &LSVec<int>::ls_map_real;
				} else {
					ls_func = (void*) &LSVec<int>::ls_map;
				}
			} else if (v1->type.getElementType(0) == Type::FLOAT) {
				if (type.getElementType(0) == Type::FLOAT) {
					ls_func = (void*) &LSVec<double>::ls_map_real;
				} else if (type.getElementType(0) == Type::INTEGER) {
					ls_func = (void*) &LSVec<double>::ls_map_int;
				} else {
					ls_func = (void*) &LSVec<double>::ls_map;
				}
			} else {
				ls_func = (void*) &LSVec<LSValue*>::ls_map;
			}
			break;
		}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
		case TokenType::IN: {
			use_jit_func = false;
			ls_func = (void*) &jit_in;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::INSTANCEOF: {
			use_jit_func = false;
			ls_func = (void*) &jit_instanceof;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::BIT_AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_bit_and;
			break;
		}
		case TokenType::BIT_AND_EQUALS: {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_and(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_and_equal;
			}
			break;
		}
		case TokenType::PIPE: {
			jit_func = &jit_insn_or;
			ls_func = (void*) &jit_bit_or;
			break;
		}
		case TokenType::BIT_OR_EQUALS: {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t o = jit_insn_or(c.F, x, y);
				jit_insn_store(c.F, x, o);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, o, type);
				}
				return o;
			} else {
				ls_func = (void*) &jit_bit_or_equal;
			}
			break;
		}
		case TokenType::BIT_XOR: {
			jit_func = &jit_insn_xor;
			ls_func = (void*) &jit_bit_xor;
			break;
		}
		case TokenType::BIT_XOR_EQUALS: {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_xor(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_xor_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_LEFT : {
			jit_func = &jit_insn_shl;
			ls_func = (void*) &jit_bit_shl;
			break;
		}
		case TokenType::BIT_SHIFT_LEFT_EQUALS : {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_shl(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shl_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT : {
			jit_func = &jit_insn_shr;
			ls_func = (void*) &jit_bit_shr;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_EQUALS : {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_shr(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_equal;
			}
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED : {
			jit_func = &jit_insn_ushr;
			ls_func = (void*) &jit_bit_shr_unsigned;
			break;
		}
		case TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS : {
			if (v1->type.raw_type->nature() == Nature::VALUE and v2->type.raw_type->nature() == Nature::VALUE) {
				jit_value_t x = v1->compile(c);
				jit_value_t y = v2->compile(c);
				jit_value_t a = jit_insn_ushr(c.F, x, y);
				jit_insn_store(c.F, x, a);
				if (v2->type.raw_type->nature() != Nature::LSVALUE and type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, a, type);
				}
				return a;
			} else {
				ls_func = (void*) &jit_bit_shr_unsigned_equal;
			}
			break;
		}
		case TokenType::DOUBLE_QUESTION_MARK: {

			// x ?? y ==> if (x != null) { x } else { y }

			jit_label_t label_end = jit_label_undefined;
			jit_label_t label_else = jit_label_undefined;
			jit_value_t v = jit_value_create(c.F, LS_POINTER);

			jit_type_t args_types[2] = {LS_POINTER};
			jit_value_t x = v1->compile(c);
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_I32, args_types, 1, 0);
			jit_value_t r = jit_insn_call_native(c.F, "is_null", (void*) jit_is_null, sig, &x, 1, JIT_CALL_NOTHROW);

			jit_insn_branch_if(c.F, r, &label_else);

			// then {
			jit_insn_store(c.F, v, x);
//			VM::inc_refs(c.F, x);

			// else
			jit_insn_branch(c.F, &label_end);
			jit_insn_label(c.F, &label_else);
			// {

			jit_value_t y = v2->compile(c);
			jit_insn_store(c.F, v, y);
//			VM::inc_refs(c.F, y);

			jit_insn_label(c.F, &label_end);

			return v;
			break;
		}
		default: {
			throw new exception();
		}
		}

	if (use_jit_func) {

		jit_value_t x = v1->compile(c);
		jit_value_t y = v2->compile(c);
		jit_value_t r = jit_func(c.F, x, y);

		if (type.raw_type->nature() == Nature::LSVALUE) {
			return VM::value_to_pointer(c.F, r, jit_returned_type);
		}
		return r;

	} else {

		jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

		if (args.size() == 0) {
			args.push_back(v1->compile(c));
			args.push_back(v2->compile(c));
		}
		jit_value_t v = jit_insn_call_native(c.F, "", ls_func, sig, args.data(), 2, JIT_CALL_NOTHROW);

		if (store_result_in_v1) {
			jit_insn_store(c.F, args[0], v);
		}

		if (type.raw_type->nature() == Nature::LSVALUE && ls_returned_type.raw_type->nature() != Nature::LSVALUE) {
			return VM::value_to_pointer(c.F, v, ls_returned_type);
		}

		return v;
	}
*/
}

}

