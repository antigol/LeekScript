#include "../../compiler/value/Expression.hpp"

#include "../../compiler/value/ArrayAccess.hpp"
#include "../../compiler/value/ObjectAccess.hpp"
#include "../../compiler/value/Function.hpp"
#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../compiler/value/VariableValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSBoolean.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/Program.hpp"

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

	if (parenthesis or debug) {
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
	if (parenthesis or debug) {
		os << ")";
	}
	if (debug) {
		os << " " << type;
	}
}

unsigned Expression::line() const {
	return 0;
}

void Expression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	operations = 1;
	type = Type::VALUE;

	// No operator : just analyse v1 and return
	if (op == nullptr) {
		v1->analyse(analyser, req_type);
		type = v1->type;
		return;
	}

	v1->analyse(analyser, Type::UNKNOWN);
	v2->analyse(analyser, Type::UNKNOWN);

	this->v1_type = op->reversed ? v2->type : v1->type;
	this->v2_type = op->reversed ? v1->type : v2->type;

	LSClass* object_class = (LSClass*) analyser->program->system_vars[this->v1_type.clazz];
	LSClass::Operator* m = nullptr;

	if (object_class) {
		// Search in the object class first
		m = object_class->getOperator(op->character, this->v1_type, this->v2_type);
		if (m == nullptr) {
			// Search in the Value class if not found
			auto value_class = (LSClass*) analyser->program->system_vars["Value"];
			m = value_class->getOperator(op->character, this->v1_type, this->v2_type);
		}
	}

	if (m != nullptr) {

		//std::cout << "Operator " << v1->type << " " << op->character << " " << v2->type << " found!" << std::endl;

		operator_fun = m->addr;
		is_native_method = m->native;
		this->v1_type = op->reversed ? m->operand_type : m->object_type;
		this->v2_type = op->reversed ? m->object_type : m->operand_type;
		return_type = m->return_type;
		type = return_type;
		if (v1->type != this->v1_type) {
			v1->analyse(analyser, this->v1_type);
		}
		if (v2->type != this->v2_type) {
			v2->analyse(analyser, this->v2_type);
		}
		if (req_type.nature == Nature::POINTER) {
			type.nature = req_type.nature;
		}
		return;
	} else {
		//std::cout << "No such operator " << v1->type << " " << op->character << " " << v2->type << std::endl;
	}

	// Don't use old stuff for boolean
//	if (v1->type == Type::BOOLEAN) {
//		analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, v1->line(), op->character});
//		return;
//	}

	if (op->type == TokenType::IN) {
		if (operator_fun == nullptr) {
			analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, v1->line(), op->character});
			return;
		}
	}

	/*
	 * OLD
	 */

	// Require float type for a division
	if (op->type == TokenType::DIVIDE) {
		type.raw_type = RawType::REAL;
		v1->analyse(analyser, Type::REAL);
	}

	if (v1->type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
	if (v2->type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
	constant = v1->constant and v2->constant;

	bool array_push = v1->type.raw_type == RawType::ARRAY and op->type == TokenType::PLUS_EQUAL;

	// array += ?  ==>  will take the type of ?
	if (array_push) {
		v1->will_store(analyser, v2->type);
		type = v2->type;
	}

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
		or op->type == TokenType::SWAP or op->type == TokenType::INT_DIV) {

		// Set the correct type nature for the two members
		if (array_push) {
			if (v1->type.getElementType().nature == Nature::POINTER and v2->type.nature != Nature::POINTER) {
				v2->analyse(analyser, Type::POINTER);
			}
		} else {
			if (v2->type.nature == Nature::POINTER and v1->type.nature != Nature::POINTER) {
				v1->analyse(analyser, Type::POINTER);
			}
			if (v1->type.nature == Nature::POINTER and v2->type.nature != Nature::POINTER) {
				v2->analyse(analyser, Type::POINTER);
			}
		}
		type = v1->type.mix(v2->type);

		if (op->type == TokenType::DIVIDE and v1->type.isNumber() and v2->type.isNumber()) {
			type.raw_type = RawType::REAL;
		}

		// String / String => Array<String>
		if (op->type == TokenType::DIVIDE and (v1->type == Type::STRING or v1->type == Type::STRING_TMP) and (v2->type == Type::STRING or v2->type == Type::STRING_TMP)) {
			type = Type::STRING_ARRAY;
		}
	}

	// Boolean operators : result is a boolean
	if (op->type == TokenType::GREATER or op->type == TokenType::DOUBLE_EQUAL
		or op->type == TokenType::LOWER or op->type == TokenType::LOWER_EQUALS
		or op->type == TokenType::GREATER_EQUALS or op->type == TokenType::TRIPLE_EQUAL
		or op->type == TokenType::DIFFERENT or op->type == TokenType::TRIPLE_DIFFERENT) {

		// Set the correct type nature for the two members
		if (v2->type.nature == Nature::POINTER and v1->type.nature != Nature::POINTER) {
			v1->analyse(analyser, Type::POINTER);
		}
		if (v1->type.nature == Nature::POINTER and v2->type.nature != Nature::POINTER) {
			v2->analyse(analyser, Type::POINTER);
		}
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
		if (not v1->isLeftValue()) {
			std::string c = "<v>";
			analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_LVALUE, v1->line(), c});
			is_left_value = false;
		}

		// A += B, A -= B
		if (is_left_value and (op->type == TokenType::PLUS_EQUAL or op->type == TokenType::MINUS_EQUAL)) {
			if (v1->type == Type::INTEGER and v2->type == Type::REAL) {
				((LeftValue*) v1)->change_type(analyser, Type::REAL);
			}
		}

		store_result_in_v1 = true;
	}

	// "hello" + ? => string
	if (op->type == TokenType::PLUS and (v1->type == Type::STRING || v1->type == Type::STRING_TMP)) {
		type = Type::STRING_TMP;
	}

	// [1, 2, 3] ~~ x -> x ^ 2
	if (op->type == TokenType::TILDE_TILDE) {
		v2->will_take(analyser, { v1->type.getElementType() });
		type = Type::PTR_ARRAY;
		type.setElementType(v2->type.getReturnType());
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = Type::POINTER;
		if (v1->type == v2->type) type = v1->type;
	}

	// int div => result is int
	if (op->type == TokenType::INT_DIV) {
		type = v1->type == Type::LONG ? Type::LONG : Type::INTEGER;
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
	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
	if (req_type.raw_type == RawType::REAL) {
		conversion = type;
		type.raw_type = RawType::REAL;
	}
}

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
int jit_int_div(LSValue* x, LSValue* y) {
	LSValue* res = x->ls_int_div(y);
	int v = ((LSNumber*) res)->value;
	LSValue::delete_temporary(res);
	return v;
}
LSValue* jit_pow(LSValue* x, LSValue* y) {
	return x->ls_pow(y);
}
LSValue* jit_mod(LSValue* x, LSValue* y) {
	return x->ls_mod(y);
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
int jit_array_push_int(LSArray<int>* x, int y) {
	x->push_move(y);
	return y;
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
LSValue* jit_bit_and(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_and_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_or(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_or_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_xor(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_xor_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shl(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shl_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shr(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shr_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shr_unsigned(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
LSValue* jit_bit_shr_unsigned_equal(LSValue* x, LSValue* y) {
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return LSNull::get();
}
bool jit_is_null(LSValue* v) {
	return v->typeID() == 1;
}

Compiler::value Expression::compile(Compiler& c) const {

	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	// Increment operations
	if (operations > 0) {
		VM::inc_ops(c.F, operations);
	}

	if (operator_fun != nullptr) {

		vector<Compiler::value> args = {
			op->reversed ? v2->compile(c) : v1->compile(c),
			op->reversed ? v1->compile(c) : v2->compile(c)
		};

		Compiler::value res;
		if (is_native_method) {
			auto fun = (Compiler::value (*)(Compiler&, std::vector<Compiler::value>)) operator_fun;
			res = fun(c, args);
		} else {
			auto fun = (void*) operator_fun;
			res = c.insn_call(type, args, fun);
		}
		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, type), type};
		}
		return res;
	}

//	cout << "v1 : " << v1->type << ", v2 : " << v2->type << endl;

	jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
	void* ls_func;
	bool use_jit_func = v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE;
	vector<jit_value_t> args;
	Type jit_returned_type = Type::UNKNOWN;
	Type ls_returned_type = Type::POINTER; // By default ls_ function returns pointers

	switch (op->type) {
		case TokenType::EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {

				if (ArrayAccess* l1 = dynamic_cast<ArrayAccess*>(v1)) {

					args.push_back(l1->compile_l(c).v);
					args.push_back(v2->compile(c).v);

					jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
					jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 0);
					jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_store_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

					if (type.nature == Nature::POINTER) {
						return {VM::value_to_pointer(c.F, v, type), type};
					}
					return {v, type};
				} else {

					auto x = v1->compile(c);
					auto y = v2->compile(c);
					jit_insn_store(c.F, x.v, y.v);
					if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
						return {VM::value_to_pointer(c.F, y.v, type), type};
					}
					return y;
				}
			} else if (v1->type.nature == Nature::POINTER) {

				if (dynamic_cast<ArrayAccess*>(v1)) {

					args.push_back(((LeftValue*) v1)->compile_l(c).v);
					args.push_back(v2->compile(c).v);

					if (v1->type.must_manage_memory()) {
						args[1] = VM::move_inc_obj(c.F, args[1]);
					}

					ls_func = (void*) &jit_store;

				} else if (dynamic_cast<VariableValue*>(v1)) {

					auto x = v1->compile(c);
					auto y = v2->compile(c);

					if (v2->type.must_manage_memory()) {
						y.v = VM::move_inc_obj(c.F, y.v);
					}
					if (v1->type.must_manage_memory()) {
						VM::delete_ref(c.F, x.v);
					}
					jit_insn_store(c.F, x.v, y.v);
					return y;

				} else {
					args.push_back(((LeftValue*) v1)->compile_l(c).v);
					args.push_back(v2->compile(c).v);
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t t = jit_insn_load(c.F, x.v);
				jit_insn_store(c.F, x.v, y.v);
				jit_insn_store(c.F, y.v, t);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, x.v, type), type};
				}
				return x;
			} else {
				args.push_back(((LeftValue*) v1)->compile_l(c).v);
				args.push_back(((LeftValue*) v2)->compile_l(c).v);
				ls_func = (void*) &jit_swap;
			}
			break;
		}
		case TokenType::PLUS_EQUAL: {

			if (v1->type == Type::INT_ARRAY and v2->type == Type::INTEGER) {
				args.push_back(v1->compile(c).v);
				args.push_back(v2->compile(c).v);
				jit_value_t res = VM::call(c.F, LS_INTEGER, {LS_POINTER, LS_INTEGER}, args, &jit_array_push_int);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, res, type), type};
				}
				return {res, type};
			}

			if (ArrayAccess* l1 = dynamic_cast<ArrayAccess*>(v1)) {

				args.push_back(l1->compile_l(c).v);
				args.push_back(v2->compile(c).v);

				jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 0);
				jit_value_t v = jit_insn_call_native(c.F, "", (void*) jit_add_equal_value, sig, args.data(), 2, JIT_CALL_NOTHROW);

				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, v, type), type};
				}
				return {v, type};
			}

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t sum = jit_insn_add(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_add_equal;
			}
			break;
		}
		case TokenType::MINUS_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t sum = jit_insn_sub(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_sub_equal;
			}
			break;
		}
		case TokenType::TIMES_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {

				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t sum = jit_insn_mul(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_mul_equal;
			}
			break;
		}
		case TokenType::DIVIDE_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t xf = jit_value_create(c.F, LS_REAL);
				jit_insn_store(c.F, xf, x.v);
				jit_value_t sum = jit_insn_div(c.F, xf, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_div_equal;
			}
			break;
		}
		case TokenType::MODULO_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t sum = jit_insn_rem(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_mod_equal;
			}
			break;
		}
		case TokenType::POWER_EQUAL: {

			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				jit_value_t sum = jit_insn_pow(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				ls_func = (void*) &jit_pow_equal;
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
			jit_returned_type = Type::REAL;
			break;
		}
		case TokenType::INT_DIV: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto r = jit_insn_floor(c.F, jit_insn_div(c.F, x.v, y.v));
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, Type::INTEGER), type};
				}
				return {r, type};
			} else {
				ls_func = (void*) &jit_int_div;
			}
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

		case TokenType::LOWER_EQUALS: {
			if (use_jit_func) {
				if (v1->type == Type::BOOLEAN && v2->type.isNumber()) {
					if (type.nature == Nature::VALUE) return c.new_bool(true);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(true)), type};
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.nature == Nature::VALUE) return c.new_bool(false);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(false)), type};
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
					if (type.nature == Nature::VALUE) return c.new_bool(false);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(false)), type};
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.nature == Nature::VALUE) return c.new_bool(true);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(true)), type};
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
					if (type.nature == Nature::VALUE) return c.new_bool(false);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(false)), type};
				}
				if (v1->type.isNumber() && v2->type == Type::BOOLEAN) {
					if (type.nature == Nature::VALUE) return c.new_bool(true);
					else return {LS_CREATE_POINTER(c.F, LSBoolean::get(true)), type};
				}
			}
			jit_func = &jit_insn_ge;
			ls_func = (void*) &jit_ge;
			jit_returned_type = Type::BOOLEAN;
			ls_returned_type = Type::BOOLEAN;
			break;
		}
		case TokenType::TILDE_TILDE: {
			if (v1->type.getElementType() == Type::INTEGER) {
				if (type.getElementType() == Type::INTEGER) {
					ls_func = (void*) &LSArray<int>::ls_map_int;
				} else if (type.getElementType() == Type::REAL) {
					ls_func = (void*) &LSArray<int>::ls_map_real;
				} else {
					ls_func = (void*) &LSArray<int>::ls_map;
				}
			} else if (v1->type.getElementType() == Type::REAL) {
				if (type.getElementType() == Type::REAL) {
					ls_func = (void*) &LSArray<double>::ls_map_real;
				} else if (type.getElementType() == Type::INTEGER) {
					ls_func = (void*) &LSArray<double>::ls_map_int;
				} else {
					ls_func = (void*) &LSArray<double>::ls_map;
				}
			} else {
				ls_func = (void*) &LSArray<LSValue*>::ls_map;
			}
			break;
		}
		case TokenType::BIT_AND: {
			jit_func = &jit_insn_and;
			ls_func = (void*) &jit_bit_and;
			break;
		}
		case TokenType::BIT_AND_EQUALS: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto a = jit_insn_and(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, a, type), type};
				}
				return {a, type};
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto o = jit_insn_or(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, o);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, o, type), type};
				}
				return {o, type};
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto a = jit_insn_xor(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, a, type), type};
				}
				return {a, type};
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto a = jit_insn_shl(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, a, type), type};
				}
				return {a, type};
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto a = jit_insn_shr(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, a, type), type};
				}
				return {a, type};
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
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				auto a = jit_insn_ushr(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, a);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, a, type), type};
				}
				return {a, type};
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
			auto x = v1->compile(c);
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 0);
			jit_value_t r = jit_insn_call_native(c.F, "is_null", (void*) jit_is_null, sig, &x.v, 1, JIT_CALL_NOTHROW);

			jit_insn_branch_if(c.F, r, &label_else);

			// then {
			jit_insn_store(c.F, v, x.v);
//			VM::inc_refs(c.F, x);

			// else
			jit_insn_branch(c.F, &label_end);
			jit_insn_label(c.F, &label_else);
			// {

			auto y = v2->compile(c);
			jit_insn_store(c.F, v, y.v);
//			VM::inc_refs(c.F, y);

			jit_insn_label(c.F, &label_end);

			return {v, type};
			break;
		}
		default: {
			throw new exception();
		}
		}

	if (use_jit_func) {

		auto x = v1->compile(c);
		auto y = v2->compile(c);
		auto r = jit_func(c.F, x.v, y.v);

		if (type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, r, jit_returned_type), type};
		}
		if (conversion == Type::INTEGER and type.raw_type == RawType::REAL) {
			return {VM::int_to_real(c.F, r), type};
		}
		return {r, type};

	} else {

		jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

		if (args.size() == 0) {
			args.push_back(v1->compile(c).v);
			args.push_back(v2->compile(c).v);
		}
		jit_value_t v = jit_insn_call_native(c.F, "", ls_func, sig, args.data(), 2, 0);

		if (store_result_in_v1) {
			jit_insn_store(c.F, args[0], v);
		}

		if (type.nature == Nature::POINTER && ls_returned_type.nature != Nature::POINTER) {
			return {VM::value_to_pointer(c.F, v, ls_returned_type), type};
		}

		return {v, type};
	}
}

}
