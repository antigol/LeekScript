#include "Expression.hpp"
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
#include "../../vm/standard/BooleanSTD.hpp"
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
	if (v2 != nullptr) {
		delete v2;
	}
}

void Expression::append(std::shared_ptr<Operator> op, Value* exp) {

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
		auto ex = new Expression();
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
		auto newV1 = new Expression();
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

Location Expression::location() const {
	auto start = v1->location().start;
	auto end = op == nullptr ? v1->location().end : v2->location().end;
	return {start, end};
}

void Expression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	operator_fun = nullptr;
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

	if (dynamic_cast<const PlaceholderRawType*>(v1->type.raw_type)) {
		type = v1->type;
		types = type;
		return;
	}
	if (dynamic_cast<const PlaceholderRawType*>(v2->type.raw_type)) {
		type = v2->type;
		types = type;
		return;
	}

	// in operator : v1 must be a container
	if (op->type == TokenType::IN and v2->type.raw_type != RawType::UNKNOWN and !v2->type.is_container()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, location(), v2->location(), {v2->to_string()}});
		return;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::POWER_EQUAL or op->type == TokenType::INT_DIV_EQUAL) {
		// TODO other operators like |= ^= &=
		if (v1->type.constant) {
			analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), op->token->location, {v1->to_string()}});
			return; // don't analyse more
		}
		// Check if A is a l-value
		if (not v1->isLeftValue()) {
			analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_LVALUE, location(), v1->location(), {v1->to_string()}});
			return; // don't analyse more
		}
		// Change the type of x for operator =
		equal_previous_type = v1->type;
		auto vv = dynamic_cast<VariableValue*>(v1);
		if (vv and op->type == TokenType::EQUAL) {
			if (v1->type != v2->type.not_temporary()) {
				auto left_v1 = static_cast<LeftValue*>(v1);
				left_v1->change_type(analyser, v2->type.not_temporary());
			}
		}
	}

	this->v1_type = op->reversed ? v2->type : v1->type;
	this->v2_type = op->reversed ? v1->type : v2->type;

	LSClass* object_class = (LSClass*) analyser->vm->system_vars[this->v1_type.clazz];
	LSClass::Operator* m = nullptr;

	if (object_class) {
		// Search in the object class first
		m = object_class->getOperator(op->character, this->v1_type, this->v2_type);
	}
	if (m == nullptr) {
		// Search in the Value class if not found
		auto value_class = (LSClass*) analyser->vm->system_vars["Value"];
		m = value_class->getOperator(op->character, this->v1_type, this->v2_type);
	}

	if (m != nullptr) {

		// std::cout << "Operator " << v1->to_string() << " (" << v1->type << ") " << op->character << " " << v2->to_string() << "(" << v2->type << ") found! " << (void*) m->addr << std::endl;

		operator_fun = m->addr;
		is_native_method = m->native;
		this->v1_type = op->reversed ? m->operand_type : m->object_type;
		this->v2_type = op->reversed ? m->object_type : m->operand_type;
		return_type = m->return_type;
		type = return_type;
		if (v1->type.not_temporary() != this->v1_type.not_temporary()) {
			v1->analyse(analyser, this->v1_type);
		}
		if (v2->type.not_temporary() != this->v2_type.not_temporary()) {
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
	if (v1->type == Type::BOOLEAN) {
		analyser->add_error({SemanticError::Type::NO_SUCH_OPERATOR, location(), location(), {v1->type.to_string(), op->character, v2->type.to_string()}});
		return;
	}

	/*
	 * OLD
	 */
	if (v1->type.nature == Nature::POINTER or v2->type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
	constant = v1->constant and v2->constant;

	bool array_push = (v1->type.raw_type == RawType::ARRAY or v1->type.raw_type == RawType::SET) and op->type == TokenType::PLUS_EQUAL;

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
		or op->type == TokenType::SWAP or op->type == TokenType::INT_DIV
		or op->type == TokenType::INT_DIV_EQUAL
		or op->type == TokenType::BIT_AND_EQUALS or op->type == TokenType::BIT_OR_EQUALS
		or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::BIT_SHIFT_LEFT or op->type == TokenType::BIT_SHIFT_LEFT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT or op->type == TokenType::BIT_SHIFT_RIGHT_EQUALS
		or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED or op->type == TokenType::BIT_SHIFT_RIGHT_UNSIGNED_EQUALS or op->type == TokenType::CATCH_ELSE
		) {

		auto vv = dynamic_cast<VariableValue*>(v1);

		// Set the correct type nature for the two members
		if (array_push) {
			if (v1->type.getElementType().nature == Nature::POINTER and v2->type.nature != Nature::POINTER) {
				v2->analyse(analyser, Type::POINTER);
			}
		} else {
			if (v2->type.nature == Nature::POINTER and v1->type.nature != Nature::POINTER and !(vv and op->type == TokenType::EQUAL)) {
				v1->analyse(analyser, Type::POINTER);
			}
			if (v1->type.nature == Nature::POINTER and v2->type.nature != Nature::POINTER and !(vv and op->type == TokenType::EQUAL)) {
				v2->analyse(analyser, Type::POINTER);
			}
		}

		if (op->type == TokenType::EQUAL and vv != nullptr) {
			type = v2->type;
		} else {
			type = v1->type.mix(v2->type);
		}
		if (type == Type::MPZ) {
			type.temporary = true;
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

		if (op->type == TokenType::EQUAL) {
			if (v1->type.not_temporary() != v2->type.not_temporary()) {
				((LeftValue*) v1)->change_type(analyser, v2->type);
			}
		} else {
			if (!array_push && Type::more_specific(v2->type, v1->type)) {
				((LeftValue*) v1)->change_type(analyser, v2->type);
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
		auto version = { v1->type.getElementType() };
		v2->will_take(analyser, version, 1);
		v2->set_version(version);
		type = Type::PTR_ARRAY;
		type.setElementType(v2->version_type(version).getReturnType());
	}

	// object ?? default
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = Type::POINTER;
		if (v1->type == v2->type) type = v1->type;
	}

	// int div => result is int
	if (op->type == TokenType::INT_DIV or op->type == TokenType::INT_DIV_EQUAL) {
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
	if (req_type.nature == Nature::VOID) {
		type = Type::VOID;
	}
	if (req_type.raw_type == RawType::REAL) {
		type.raw_type = RawType::REAL;
	}
}

LSValue* jit_add(LSValue* x, LSValue* y) {
	return x->add(y);
}
LSValue* jit_sub(LSValue* x, LSValue* y) {
	return x->sub(y);
}
LSValue* jit_mul(LSValue* x, LSValue* y) {
	return x->mul(y);
}
LSValue* jit_div(LSValue* x, LSValue* y) {
	return x->div(y);
}
long jit_int_div(LSValue* x, LSValue* y) {
	LSValue* res = x->int_div(y);
	long v = ((LSNumber*) res)->value;
	LSValue::delete_temporary(res);
	return v;
}
LSValue* jit_pow(LSValue* x, LSValue* y) {
	return x->pow(y);
}
LSValue* jit_mod(LSValue* x, LSValue* y) {
	return x->mod(y);
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

LSValue* jit_swap(LSValue** x, LSValue** y) {
	LSValue* tmp = *x;
	*x = *y;
	*y = tmp;
	return *x;
}

LSValue* jit_mod_equal(LSValue* x, LSValue* y) {
	return x->mod_eq(y);
}
LSValue* jit_pow_equal(LSValue* x, LSValue* y) {
	return x->pow_eq(y);
}
int jit_bit_and_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value & (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_or_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value | (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_xor_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value ^ (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shl(LSValue* x, LSValue* y) {
	int r = (int) ((LSNumber*) x)->value << (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shl_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value << (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr(LSValue* x, LSValue* y) {
	int r = (int) ((LSNumber*) x)->value >> (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (int) ((LSNumber*) x)->value >> (int) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_unsigned(LSValue* x, LSValue* y) {
	int r = (uint32_t) ((LSNumber*) x)->value >> (uint32_t) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
int jit_bit_shr_unsigned_equal(LSValue* x, LSValue* y) {
	int r = ((LSNumber*) x)->value = (uint32_t) ((LSNumber*) x)->value >> (uint32_t) ((LSNumber*) y)->value;
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
}
bool jit_is_null(LSValue* v) {
	return v->type == LSValue::NULLL;
}

Compiler::value Expression::compile(Compiler& c) const {

	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	jit_insn_mark_offset(c.F, location().start.line);

	// Increment operations
	if (operations > 0) {
		c.inc_ops(operations);
	}

	if (operator_fun != nullptr) {

		vector<Compiler::value> args;
		if (v1->type == Type::BOOLEAN and op->type == TokenType::EQUAL) {
			args.push_back(((LeftValue*) v1)->compile_l(c));
			args.push_back(v2->compile(c));
		} else {
			args.push_back(op->reversed ? v2->compile(c) : v1->compile(c));
			args.push_back(op->reversed ? v1->compile(c) : v2->compile(c));
		}
		v1->compile_end(c);
		v2->compile_end(c);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) operator_fun;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, std::vector<Compiler::value>)) operator_fun;
			res = fun(c, args);
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
			// array[] = 12, array push
			auto array_access = dynamic_cast<ArrayAccess*>(v1);
			if (array_access && array_access->key == nullptr) {
				auto x_addr = ((LeftValue*) array_access->array)->compile_l(c);
				auto y = c.insn_to_pointer(v2->compile(c));
				v2->compile_end(c);
				return c.insn_call(Type::POINTER, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->add_eq(y);
				});
			}

			// Reference, like : a = @b
			auto varval = dynamic_cast<VariableValue*>(v1);
			if (varval != nullptr and varval->scope == VarScope::LOCAL and c.get_var(varval->name).t.reference) {

				if (v1->type.must_manage_memory()) {
					auto v1v = v1->compile(c);
					v1->compile_end(c);
					c.insn_delete(v1v);
				}
				jit_value_t var = jit_value_create(c.F, VM::get_jit_type(v2->type));
				c.add_var(varval->name, var, v2->type, false);
				auto v2_value = v2->compile(c);
				v2->compile_end(c);
				if (v2->type.must_manage_memory()) {
					v2_value = c.insn_move_inc(v2_value);
				}
				jit_insn_store(c.F, var, v2_value.v);

				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, v2_value.v, type), type};
				}
				return v2_value;
			}

			auto vv = dynamic_cast<VariableValue*>(v1);
			if (vv != nullptr and equal_previous_type != v1->type) {

				// std::cout << v1->to_string() << " (" << v1->type << ") = " << v2->to_string() << " (" << v2->type << ")" << std::endl;

				// we have a variable, like
				// var a = 12 a = 'hello' or var a = 12 a = 200l
				// create a new variable a and replace the old one

				// Delete previous variable reference
				if (equal_previous_type.must_manage_memory()) {
					auto v = c.get_var(vv->name);
					c.insn_delete(v);
				}

				auto y = v2->compile(c);
				v2->compile_end(c);

				// Move the object
				y = c.insn_move_inc(y);

				// Create a new variable
				jit_value_t var = jit_value_create(c.F, VM::get_jit_type(v1->type));
				c.update_var(vv->name, var, v1->type);

				// Store
				jit_insn_store(c.F, var, y.v);

				return y;

			} else if (equal_previous_type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto y = v2->compile(c);
				v2->compile_end(c);
				if (v1->type == Type::MPZ) {
					auto x = v1->compile(c);
					v1->compile_end(c);
					auto a = c.insn_address_of(x);
					auto b = c.insn_address_of(y);
					c.insn_call(Type::VOID, {a, b}, &mpz_set);
					if (y.t.temporary) {
						return y;
					} else {
						auto r = c.new_mpz();
						auto r_addr = c.insn_address_of(r);
						c.insn_call(Type::VOID, {r_addr, b}, &mpz_set);
						return r;
					}
				} else {
					auto v1_addr = ((LeftValue*) v1)->compile_l(c);
					jit_insn_store_relative(c.F, v1_addr.v, 0, y.v);
					if (type.nature == Nature::POINTER) {
						return {VM::value_to_pointer(c.F, y.v, type), type};
					}
					return y;
				}
			} else if (equal_previous_type.nature == Nature::POINTER) {
				auto x = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				c.insn_call(Type::VOID, {x, y}, (void*) +[](LSValue** x, LSValue* y) {
					LSValue::delete_ref(*x);
					*x = y->move_inc();
				});
				return y;
			} else {
				std::cout << "Invalid " << v1->to_string() << " (" << equal_previous_type << " => " << v1->type << ") = " << v2->to_string() << " (" << v2->type << ")" << std::endl; // LCOV_EXCL_LINE
				assert(false); // LCOV_EXCL_LINE
			}
		}
		case TokenType::SWAP: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y_addr = ((LeftValue*) v2)->compile_l(c);
				auto x = c.insn_load(x_addr, 0, v1->type);
				auto y = c.insn_load(y_addr, 0, v2->type);
				jit_value_t t = jit_insn_load(c.F, x.v);
				jit_insn_store_relative(c.F, x_addr.v, 0, y.v);
				jit_insn_store_relative(c.F, y_addr.v, 0, t);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, y.v, type), type};
				}
				return y;
			} else {
				args.push_back(((LeftValue*) v1)->compile_l(c).v);
				args.push_back(((LeftValue*) v2)->compile_l(c).v);
				ls_func = (void*) &jit_swap;
			}
			break;
		}
		case TokenType::PLUS_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				auto x = c.insn_load(x_addr, 0, v1->type);
				auto sum = c.insn_add(x, y);
				jit_insn_store_relative(c.F, x_addr.v, 0, sum.v);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum.v, type), type};
				}
				return sum;
			} else {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				v1->compile_end(c);
				auto y = c.insn_to_pointer(v2->compile(c));
				v2->compile_end(c);
				return c.insn_call(Type::POINTER, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->add_eq(y);
				});
			}
			break;
		}
		case TokenType::MINUS_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				jit_value_t sum = jit_insn_sub(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				return c.insn_call(Type::POINTER, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->sub_eq(y);
				});
			}
			break;
		}
		case TokenType::TIMES_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				jit_value_t sum = jit_insn_mul(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				auto x = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				return c.insn_call(Type::POINTER, {x, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->mul_eq(y);
				});
			}
			break;
		}
		case TokenType::DIVIDE_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
				jit_value_t xf = jit_value_create(c.F, LS_REAL);
				jit_insn_store(c.F, xf, x.v);
				jit_value_t sum = jit_insn_div(c.F, xf, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				return c.insn_call(Type::POINTER, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					return (*x)->div_eq(y);
				});
			}
			break;
		}
		case TokenType::MODULO_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x = v1->compile(c);
				auto y = v2->compile(c);
				v1->compile_end(c);
				v2->compile_end(c);
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
				v1->compile_end(c);
				v2->compile_end(c);
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
				v1->compile_end(c);
				v2->compile_end(c);
				auto r = jit_insn_convert(c.F, jit_insn_floor(c.F, jit_insn_div(c.F, x.v, y.v)), VM::get_jit_type(type), 0);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, type), type};
				}
				return {r, type};
			} else {
				ls_func = (void*) &jit_int_div;
				ls_returned_type = Type::LONG;
			}
			break;
		}
		case TokenType::INT_DIV_EQUAL: {
			if (v1->type.nature == Nature::VALUE and v2->type.nature == Nature::VALUE) {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				auto x = c.insn_load(x_addr, 0, v1->type);
				auto r = jit_insn_convert(c.F, jit_insn_floor(c.F, jit_insn_div(c.F, x.v, y.v)), VM::get_jit_type(type), 0);
				jit_insn_store_relative(c.F, x_addr.v, 0, r);
				if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, type), type};
				}
				return {r, type};
			} else {
				auto x_addr = ((LeftValue*) v1)->compile_l(c);
				auto y = v2->compile(c);
				v2->compile_end(c);
				return c.insn_call(type, {x_addr, y}, (void*) +[](LSValue** x, LSValue* y) {
					LSValue* res = (*x)->int_div_eq(y);
					long v = ((LSNumber*) res)->value;
					return v;
				});
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
		case TokenType::TILDE_TILDE: {
			if (v1->type.getElementType() == Type::INTEGER) {
				if (type.getElementType() == Type::INTEGER) {
					auto m = &LSArray<int>::ls_map<int>;
					ls_func = (void*) m;
				} else if (type.getElementType() == Type::REAL) {
					auto m = &LSArray<int>::ls_map<double>;
					ls_func = (void*) m;
				} else {
					auto m = &LSArray<int>::ls_map<LSValue*>;
					ls_func = (void*) m;
				}
			} else if (v1->type.getElementType() == Type::REAL) {
				if (type.getElementType() == Type::REAL) {
					auto m = &LSArray<double>::ls_map<double>;
					ls_func = (void*) m;
				} else if (type.getElementType() == Type::INTEGER) {
					auto m = &LSArray<double>::ls_map<int>;
					ls_func = (void*) m;
				} else {
					// FIXME Ugly style to fix uncovered line
					auto m = &LSArray<double>::ls_map<LSValue*>; ls_func = (void*) m;
				}
			} else {
				auto m = &LSArray<LSValue*>::ls_map<LSValue*>;
				ls_func = (void*) m;
			}
			break;
		}
		case TokenType::BIT_AND_EQUALS : {
			ls_func = (void*) &jit_bit_and_equal;
			break;
		}
		case TokenType::BIT_OR_EQUALS : {
			ls_func = (void*) &jit_bit_or_equal;
			break;
		}
		case TokenType::BIT_XOR_EQUALS : {
			ls_func = (void*) &jit_bit_xor_equal;
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
				v1->compile_end(c);
				v2->compile_end(c);
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
				v1->compile_end(c);
				v2->compile_end(c);
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
				v1->compile_end(c);
				v2->compile_end(c);
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
			v1->compile_end(c);
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 1);
			jit_value_t r = jit_insn_call_native(c.F, "is_null", (void*) jit_is_null, sig, &x.v, 1, JIT_CALL_NOTHROW);
			jit_type_free(sig);

			jit_insn_branch_if(c.F, r, &label_else);
			// then {
			jit_insn_store(c.F, v, x.v);
			// else
			jit_insn_branch(c.F, &label_end);
			jit_insn_label(c.F, &label_else);
			// {
			auto y = v2->compile(c);
			v2->compile_end(c);
			jit_insn_store(c.F, v, y.v);

			jit_insn_label(c.F, &label_end);

			return {v, type};
			break;
		}
		case TokenType::CATCH_ELSE: {

			jit_label_t try_start = jit_label_undefined;
			jit_label_t try_end = jit_label_undefined;
			jit_label_t exception_thrown = jit_label_undefined;
			jit_label_t no_exception = jit_label_undefined;

			jit_insn_label(c.F, &try_start);
			auto r = jit_value_create(c.F, VM::get_jit_type(type));
			jit_insn_store(c.F, r, v1->compile(c).v);
			v1->compile_end(c);
			jit_insn_label(c.F, &try_end);

			jit_insn_branch(c.F, &no_exception);
			jit_insn_label(c.F, &exception_thrown);

			c.add_catcher(try_start, try_end, exception_thrown);

			auto y = v2->compile(c);
			v2->compile_end(c);
			if (v2->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
				y = {VM::value_to_pointer(c.F, y.v, v2->type), type};
			}
			jit_insn_store(c.F, r, y.v);
			jit_insn_label(c.F, &no_exception);
			return {r, type};

			break;
		}
		default: {
			std::cout << "No such operator to compile : " << op->character << " (" << (int) op->type << ")" << std::endl; // LCOV_EXCL_LINE
			assert(false); // LCOV_EXCL_LINE
		}
	}

	if (use_jit_func) {

		jit_insn_mark_offset(c.F, location().start.line);

		auto x = v1->compile(c);
		auto y = v2->compile(c);
		v1->compile_end(c);
		v2->compile_end(c);
		auto r = jit_func(c.F, x.v, y.v);

		if (type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, r, jit_returned_type), type};
		}
		return {r, type};

	} else {

		jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 1);

		if (args.size() == 0) {
			args.push_back(v1->compile(c).v);
			args.push_back(v2->compile(c).v);
			v1->compile_end(c);
			v2->compile_end(c);
		}
		jit_value_t v = jit_insn_call_native(c.F, "", ls_func, sig, args.data(), 2, 0);
		jit_type_free(sig);

		if (store_result_in_v1) {
			jit_insn_store(c.F, args[0], v);
		}

		if (type.nature == Nature::POINTER && ls_returned_type.nature != Nature::POINTER) {
			return {VM::value_to_pointer(c.F, v, ls_returned_type), type};
		}
		return {v, type};
	}
}

Value* Expression::clone() const {
	auto ex = new Expression();
	ex->v1 = v1->clone();
	ex->op = op;
	ex->v2 = v2 ? v2->clone() : nullptr;
	return ex;
}

}
