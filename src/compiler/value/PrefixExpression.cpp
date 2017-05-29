#include "PrefixExpression.hpp"
#include "FunctionCall.hpp"
#include "LeftValue.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

PrefixExpression::PrefixExpression() {
	expression = nullptr;
	operatorr = nullptr;
}

PrefixExpression::~PrefixExpression() {
	delete expression;
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

Location PrefixExpression::location() const {
	return {operatorr->token->location.start, expression->location().end};
}

void PrefixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	expression->analyse(analyser, Type::UNKNOWN);

	if (operatorr->type == TokenType::TILDE) {
		type = expression->type;
		if (type == Type::BOOLEAN) {
			type = Type::INTEGER;
		}
	}

	if (operatorr->type == TokenType::PLUS_PLUS
		or operatorr->type == TokenType::MINUS_MINUS
		or operatorr->type == TokenType::MINUS) {

		type = expression->type;
		if (type == Type::MPZ and operatorr->type == TokenType::MINUS) {
			type = Type::MPZ_TMP;
		}

		if (operatorr->type == TokenType::PLUS_PLUS or operatorr->type == TokenType::MINUS_MINUS) {
			if (expression->type.constant) {
				analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), expression->location(), {expression->to_string()}});
			}
		}

	} else if (operatorr->type == TokenType::NOT) {

		type = Type::BOOLEAN;

	} else if (operatorr->type == TokenType::NEW) {

		if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {
			if (vv->name == "Number") type = Type::INTEGER;
			else if (vv->name == "Boolean") type = Type::BOOLEAN;
			else if (vv->name == "String") type = Type::STRING;
			else if (vv->name == "Array") type = Type::PTR_ARRAY;
			else if (vv->name == "Object") type = Type::OBJECT;
			else {
				type = Type::POINTER;
			}
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

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

LSValue* jit_not(LSValue* x) {
	// TODO optimization, don't create a LSBoolean
	auto r = LSBoolean::get(x->ls_not());
	LSValue::delete_temporary(x);
	return r;
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

Compiler::value PrefixExpression::compile(Compiler& c) const {

	jit_insn_mark_offset(c.F, location().start.line);

	jit_type_t args_types[1] = {LS_POINTER};
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type == Type::MPZ) {
				auto x = expression->compile(c);
				auto x_addr = c.insn_address_of(x);
				auto one = c.new_integer(1);
				c.insn_call(Type::VOID, {x_addr, x_addr, one}, &mpz_add_ui);
				return x;
			} else if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				auto y = c.new_integer(1);
				jit_value_t sum = jit_insn_add(c.F, x.v, y.v);
				jit_insn_store(c.F, x.v, sum);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				args.push_back(expression->compile(c).v);
				func = (void*) jit_pre_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				jit_value_t y = LS_CREATE_INTEGER(c.F, 1);
				jit_value_t sum = jit_insn_sub(c.F, x.v, y);
				jit_insn_store(c.F, x.v, sum);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, sum, type), type};
				}
				return {sum, type};
			} else {
				args.push_back(expression->compile(c).v);
				func = (void*) jit_pre_dec;
			}
			break;
		}
		case TokenType::NOT: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				jit_value_t r = jit_insn_to_not_bool(c.F, x.v);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, Type::BOOLEAN), type};
				}
				return {r, type};
			} else {
				args.push_back(expression->compile(c).v);
				func = (void*) jit_not;
			}
			break;
		}
		case TokenType::MINUS: {
			if (expression->type.not_temporary() == Type::MPZ) {
				auto x = expression->compile(c);
				auto r = [&]() {
					if (x.t.temporary) {
						return x;
					} else {
						return c.new_mpz();
					}
				}();
				auto x_addr = c.insn_address_of(x);
				auto r_addr = c.insn_address_of(r);
				c.insn_call(Type::VOID, {r_addr, x_addr}, &mpz_neg);
				return r;
			} else if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				jit_value_t r = jit_insn_neg(c.F, x.v);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, type), type};
				}
				return {r, type};
			} else {
				args.push_back(expression->compile(c).v);
				func = (void*) jit_minus;
			}
			break;
		}
		case TokenType::TILDE: {
			if (expression->type.nature == Nature::VALUE) {
				auto x = expression->compile(c);
				jit_value_t r = jit_insn_not(c.F, x.v);
				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, r, type), type};
				}
				return {r, type};
			} else {
				args.push_back(expression->compile(c).v);
				func = (void*) jit_pre_tilde;
			}
			break;
		}
		case TokenType::NEW: {

			if (VariableValue* vv = dynamic_cast<VariableValue*>(expression)) {

				if (vv->name == "Number") {
					jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
					if (type.nature == Nature::POINTER) {
						return {VM::value_to_pointer(c.F, n, Type::INTEGER), type};
					}
					return {n, type};
				}
				else if (vv->name == "Boolean") {
					jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
					if (type.nature == Nature::POINTER) {
						return {VM::value_to_pointer(c.F, n, Type::BOOLEAN), type};
					}
					return {n, type};
				}
				else if (vv->name == "String") {
					return {c.new_pointer(new LSString("")).v, type};
				}
				else if (vv->name == "Array") {
					return {c.new_pointer(new LSArray<LSValue*>()).v, type};
				}
				else if (vv->name == "Object") {
					return {c.new_pointer(new LSObject()).v, type};
				}
				else {
					auto clazz = expression->compile(c);
					return c.new_object_class(clazz);
				}
			}

			if (FunctionCall* fc = dynamic_cast<FunctionCall*>(expression)) {
				if (VariableValue* vv = dynamic_cast<VariableValue*>(fc->function)) {
					if (vv->name == "Number") {
						if (fc->arguments.size() > 0) {
							auto n = fc->arguments[0]->compile(c);
							if (type.nature == Nature::POINTER) {
								return {VM::value_to_pointer(c.F, n.v, Type::INTEGER), type};
							}
							return n;
						} else {
							jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
							if (type.nature == Nature::POINTER) {
								return {VM::value_to_pointer(c.F, n, Type::INTEGER), type};
							}
							return {n, type};
						}
					}
					if (vv->name == "Boolean") {
						jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
						if (type.nature == Nature::POINTER) {
							return {VM::value_to_pointer(c.F, n, Type::BOOLEAN), type};
						}
						return {n, type};
					}
					if (vv->name == "String") {
						if (fc->arguments.size() > 0) {
							return fc->arguments[0]->compile(c);
						}
						return {c.new_pointer(new LSString("")).v, type};
					}
					if (vv->name == "Array") {
						return {c.new_pointer(new LSArray<LSValue*>()).v, type};
					}
					if (vv->name == "Object") {
						return {c.new_pointer(new LSObject()).v, type};
					}
				}
			}
		}
		default: {}
	}
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 1);
	jit_value_t result = jit_insn_call_native(c.F, "", func, sig, args.data(), 1, 0);
	jit_type_free(sig);

	if (operatorr->type == TokenType::NOT) {
		result = VM::pointer_to_value(c.F, result, Type::BOOLEAN);
	}

	return {result, type};
}

Value* PrefixExpression::clone() const {
	auto pe = new PrefixExpression();
	pe->expression = (LeftValue*) expression->clone();
	pe->operatorr = operatorr;
	return pe;
}

}
