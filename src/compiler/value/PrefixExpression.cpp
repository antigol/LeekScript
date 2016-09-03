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

void PrefixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (operatorr->type == TokenType::PLUS_PLUS
		|| operatorr->type == TokenType::MINUS_MINUS
		|| operatorr->type == TokenType::MINUS
		|| operatorr->type == TokenType::TILDE) {

		expression->analyse(analyser, req_type);
		if (!expression->type.is_arithmetic()) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
		}

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
	assert(type.is_complete() || !analyser->errors.empty());

	/*
	expression->analyse(analyser, Type::UNKNOWN);

	if (operatorr->type == TokenType::PLUS_PLUS
		or operatorr->type == TokenType::MINUS_MINUS
		or operatorr->type == TokenType::MINUS
		or operatorr->type == TokenType::TILDE) {

		type = expression->type;

	} else if (operatorr->type == TokenType::NOT) {

		type = Type::BOOLEAN;

	} else if (operatorr->type == TokenType::NEW) {

		if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {
			if (vv->name == "Number") type = Type::INTEGER;
			if (vv->name == "Boolean") type = Type::BOOLEAN;
			if (vv->name == "String") type = Type::STRING;
			if (vv->name == "Array") type = Type::PTR_ARRAY;
			if (vv->name == "Object") type = Type::OBJECT;
		}
		if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
			if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
				if (vv->name == "Number") {
					if (fc->arguments.size() > 0) {
						fc->arguments[0]->analyse(analyser, Type::UNKNOWN);
						type = fc->arguments[0]->type;
					} else {
						type = Type::INTEGER;
					}
				}
				if (vv->name == "Boolean") type = Type::BOOLEAN;
				if (vv->name == "String") type = Type::STRING;
				if (vv->name == "Array") type = Type::PTR_ARRAY;
				if (vv->name == "Object") type = Type::OBJECT;
			}
		}
	}

	if (req_type.raw_type.nature() != Nature::UNKNOWN) {
		type.raw_type.nature() = req_type.raw_type.nature();
	}
	*/
}

/*
LSValue* jit_not(LSValue* x) {
	return x->ls_not();
}
LSValue* jit_minus(LSValue* x) {
	return x->ls_minus();
}
LSValue* jit_pre_inc(LSValue* x) {
	return x->ls_preinc();
}
LSValue* jit_pre_dec(LSValue* x) {
	return x->ls_predec();
}
LSValue* jit_pre_tilde(LSValue* v) {
	return v->ls_tilde();
}
*/

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
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_insn_store(c.F, x, jit_insn_add(c.F, x, VM::create_i32(c.F, 1)));
				return x;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_preinc, { x });
			}
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_insn_store(c.F, x, jit_insn_sub(c.F, x, VM::create_i32(c.F, 1)));
				return x;
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_predec, { x });
			}
		}
		case TokenType::MINUS: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				return jit_insn_neg(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_minus, { x });
			}
		}
		case TokenType::TILDE: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				return jit_insn_not(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_minus, { x });
			}
		}
		case TokenType::NOT: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				return jit_insn_to_not_bool(c.F, x);
			} else {
				return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) PE_not, { x });
			}
		}
		default: break;
	}

	assert(0);
	return nullptr;
/*
	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t y = LS_CREATE_I32(c.F, 1);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (type.raw_type.nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_pre_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t y = LS_CREATE_I32(c.F, 1);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				if (type.raw_type.nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, sum, type);
				}
				return sum;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_pre_dec;
			}
			break;
		}
		case TokenType::NOT: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t r = jit_insn_to_not_bool(c.F, x);
				if (type.raw_type.nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, r, Type::BOOLEAN);
				}
				return r;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_not;
			}
			break;
		}
		case TokenType::MINUS: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t r = jit_insn_neg(c.F, x);
				if (type.raw_type.nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, r, type);
				}
				return r;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_minus;
			}
			break;
		}
		case TokenType::TILDE: {
			if (expression->type.raw_type.nature() == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t r = jit_insn_not(c.F, x);
				if (type.raw_type.nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, r, type);
				}
				return r;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_pre_tilde;
			}
			break;
		}
		case TokenType::NEW: {

			if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {

				if (vv->name == "Number") {
					jit_value_t n = LS_CREATE_I32(c.F, 0);
					if (type.raw_type.nature() == Nature::LSVALUE) {
						return VM::value_to_pointer(c.F, n, Type::INTEGER);
					}
					return n;
				}
				if (vv->name == "Boolean") {
					jit_value_t n = LS_CREATE_I32(c.F, 0);
					if (type.raw_type.nature() == Nature::LSVALUE) {
						return VM::value_to_pointer(c.F, n, Type::BOOLEAN);
					}
					return n;
				}
				if (vv->name == "String") {
					return LS_CREATE_POINTER(c.F, new LSString(""));
				}
				if (vv->name == "Array") {
					return LS_CREATE_POINTER(c.F, new LSVec<LSValue*>());
				}
				if (vv->name == "Object") {
					return LS_CREATE_POINTER(c.F, new LSObject());
				}
			}

			if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
				if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
					if (vv->name == "Number") {
						if (fc->arguments.size() > 0) {
							jit_value_t n = fc->arguments[0]->compile(c);
							if (type.raw_type.nature() == Nature::LSVALUE) {
								return VM::value_to_pointer(c.F, n, Type::INTEGER);
							}
							return n;
						} else {
							jit_value_t n = LS_CREATE_I32(c.F, 0);
							if (type.raw_type.nature() == Nature::LSVALUE) {
								return VM::value_to_pointer(c.F, n, Type::INTEGER);
							}
							return n;
						}
					}
					if (vv->name == "Boolean") {
						jit_value_t n = LS_CREATE_I32(c.F, 0);
						if (type.raw_type.nature() == Nature::LSVALUE) {
							return VM::value_to_pointer(c.F, n, Type::BOOLEAN);
						}
						return n;
					}
					if (vv->name == "String") {
						if (fc->arguments.size() > 0) {
							return fc->arguments[0]->compile(c);
						}
						return LS_CREATE_POINTER(c.F, new LSString(""));
					}
					if (vv->name == "Array") {
						return LS_CREATE_POINTER(c.F, new LSVec<LSValue*>());
					}
					if (vv->name == "Object") {
						return LS_CREATE_POINTER(c.F, new LSObject());
					}
				}
			}

			break;
		}
		default: {

		}
	}
	jit_value_t result = jit_insn_call_native(c.F, "", func, sig, args.data(), 1, JIT_CALL_NOTHROW);

	return result;
	*/
}

}
