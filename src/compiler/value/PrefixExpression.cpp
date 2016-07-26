#include "../../compiler/value/PrefixExpression.hpp"

#include "../../compiler/value/FunctionCall.hpp"
#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/VariableValue.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSObject.hpp"

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

void PrefixExpression::print(ostream& os) const {
	operatorr->print(os);
	expression->print(os);
}

int PrefixExpression::line() const {
	return 0;
}

void PrefixExpression::analyse(SemanticAnalyser* analyser, const Type) {

	expression->analyse(analyser);

	if (operatorr->type == TokenType::PLUS_PLUS
		or operatorr->type == TokenType::MINUS_MINUS
		or operatorr->type == TokenType::MINUS) {
		type = expression->type;
	} else if (operatorr->type == TokenType::NOT) {
		type = Type::BOOLEAN;
	}
}

LSValue* jit_not(LSValue* x) {
	return x->operator ! ();
}
LSValue* jit_minus(LSValue* x) {
	return x->operator - ();
}
LSValue* jit_pre_inc(LSValue* x) {
	return x->operator ++ ();
}
LSValue* jit_pre_dec(LSValue* x) {
	return x->operator -- ();
}
LSValue* jit_pre_tilde(LSValue* v) {
	return v->operator ~ ();
}

jit_value_t PrefixExpression::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	jit_type_t args_types[1] = {JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 1, 0);
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_add(F, x, y);
				jit_insn_store(F, x, sum);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_pre_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t y = JIT_CREATE_CONST(F, JIT_INTEGER, 1);
				jit_value_t sum = jit_insn_sub(F, x, y);
				jit_insn_store(F, x, sum);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, sum, req_type);
				}
				return sum;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_pre_dec;
			}
			break;
		}
		case TokenType::NOT: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t r = jit_insn_to_not_bool(F, x);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, r, Type::BOOLEAN);
				}
				return r;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_not;
			}
			break;
		}
		case TokenType::MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile_jit(c, F, Type::NEUTRAL);
				jit_value_t r = jit_insn_neg(F, x);
				if (req_type.nature == Nature::POINTER) {
					return VM::value_to_pointer(F, r, req_type);
				}
				return r;
			} else {
				args.push_back(expression->compile_jit(c, F, Type::NEUTRAL));
				func = (void*) jit_minus;
			}
			break;
		}
		case TokenType::TILDE: {
			args.push_back(expression->compile_jit(c, F, Type::POINTER));
			func = (void*) jit_pre_tilde;
			break;
		}
		case TokenType::NEW: {

			if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {

				if (vv->name == "Number") {
					jit_value_t n = JIT_CREATE_CONST(F, JIT_INTEGER, 0);
					if (req_type.nature == Nature::POINTER) {
						return VM::value_to_pointer(F, n, Type::INTEGER);
					}
					return n;
				}
				if (vv->name == "Boolean") {
					jit_value_t n = JIT_CREATE_CONST(F, JIT_INTEGER, 0);
					if (req_type.nature == Nature::POINTER) {
						return VM::value_to_pointer(F, n, Type::BOOLEAN);
					}
					return n;
				}
				if (vv->name == "String") {
					return JIT_CREATE_CONST_POINTER(F, new LSString(""));
				}
				if (vv->name == "Array") {
					return JIT_CREATE_CONST_POINTER(F, new LSArray<LSValue*>());
				}
				if (vv->name == "Object") {
					return JIT_CREATE_CONST_POINTER(F, new LSObject());
				}
			}

			if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
				if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
					if (vv->name == "Number") {
						if (fc->arguments.size() > 0) {
							return fc->arguments[0]->compile_jit(c, F, Type::POINTER);
						} else {
							jit_value_t n = JIT_CREATE_CONST(F, JIT_INTEGER, 0);
							if (req_type.nature == Nature::POINTER) {
								return VM::value_to_pointer(F, n, Type::INTEGER);
							}
							return n;
						}
					}
					if (vv->name == "Boolean") {
						jit_value_t n = JIT_CREATE_CONST(F, JIT_INTEGER, 0);
						if (req_type.nature == Nature::POINTER) {
							return VM::value_to_pointer(F, n, Type::BOOLEAN);
						}
						return n;
					}
					if (vv->name == "String") {
						if (fc->arguments.size() > 0) {
							return fc->arguments[0]->compile_jit(c, F, Type::POINTER);
						}
						return JIT_CREATE_CONST_POINTER(F, new LSString(""));
					}
					if (vv->name == "Array") {
						return JIT_CREATE_CONST_POINTER(F, new LSArray<LSValue*>());
					}
					if (vv->name == "Object") {
						return JIT_CREATE_CONST_POINTER(F, new LSObject());
					}
				}
			}

			break;
		}
		default: {

		}
	}
	jit_value_t result = jit_insn_call_native(F, "", func, sig, args.data(), 1, JIT_CALL_NOTHROW);

	if (expression->type.nature == Nature::POINTER) {
		VM::delete_temporary(F, args[0]);
	}

	return result;
}

}
