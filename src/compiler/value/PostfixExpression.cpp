#include "PostfixExpression.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
}

PostfixExpression::~PostfixExpression() {
	delete expression;
	delete operatorr;
}

void PostfixExpression::print(ostream& os, int indent, bool debug) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

unsigned PostfixExpression::line() const {
	return 0;
}

void PostfixExpression::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	expression->analyse(analyser, req_type);
	if (expression->type == Type::FUNCTION || expression->type == Type::TUPLE) {
		analyser->add_error({ SemanticException::TYPE_MISMATCH, expression->line() });
	}

	type = expression->type;
}

LSVar* jit_inc(LSVar* x) {
	return x->ls_postinc();
}
LSVar* jit_dec(LSVar* x) {
	return x->ls_postdec();
}

jit_value_t PostfixExpression::compile(Compiler& c) const {

	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);
	vector<jit_value_t> args;

	void* func = nullptr;

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = VM::create_i32(c.F, 1);
				jit_value_t sum = jit_insn_add(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				return ox;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_inc;
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				jit_value_t x = expression->compile(c);
				jit_value_t ox = jit_insn_load(c.F, x);
				jit_value_t y = VM::create_i32(c.F, 1);
				jit_value_t sum = jit_insn_sub(c.F, x, y);
				jit_insn_store(c.F, x, sum);
				return ox;
			} else {
				args.push_back(expression->compile(c));
				func = (void*) jit_dec;
			}
			break;
		}
		default: {}
	}
	return jit_insn_call_native(c.F, "", func, sig, args.data(), 1, JIT_CALL_NOTHROW);
}

}
