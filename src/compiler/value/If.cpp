#include "../../compiler/value/If.hpp"

#include "../semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Number.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"

using namespace std;

namespace ls {

If::If(bool ternary) {
	elze = nullptr;
	condition = nullptr;
	then = nullptr;
	type = Type::UNKNOWN;
	this->ternary = ternary;
}

If::~If() {
	delete condition;
	delete then;
	if (elze != nullptr) {
		delete elze;
	}
}

void If::print(ostream& os, int indent, bool debug) const {
	if (ternary) {
		os << "(";
		condition->print(os, indent, debug);
		os << " ? ";
		then->instructions[0]->print(os, indent, debug);
		os << " : ";
		elze->instructions[0]->print(os, indent, debug);
		os << ")";
	} else {
		os << "if ";
		condition->print(os, indent + 1, debug);
		os << " ";
		then->print(os, indent, debug);
		if (elze != nullptr) {
			os << " else ";
			elze->print(os, indent, debug);
		}
	}
	if (debug) {
		os << " " << types;
	}
}

Location If::location() const {
	return condition->location(); // TODO better
}

void If::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	types.clear();

	condition->analyse(analyser, Type::UNKNOWN);
	then->analyse(analyser, req_type);

	types.add(then->types);

	if (elze != nullptr) {

		elze->analyse(analyser, req_type);

		if (then->type == Type::VOID) { // then contains return instruction
			type = elze->type;
		} else if (elze->type == Type::VOID) { // elze contains return instruction
			type = then->type;
		} else {
			type = Type::get_compatible_type(then->type, elze->type);
		}
		types.add(elze->types);

		//then->analyse(analyser, type);

		//if (elze->type != type) {
		//	elze->analyse(analyser, type);
		//}
	} else {

		// if (req_type == Type::VOID) {
			// type = Type::VOID;
		// } else {
			//type = Type::POINTER; // Pointer because the else will give null
		// }
		//then->analyse(analyser, type);
	}
	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
}

Compiler::value If::compile(Compiler& c) const {

	jit_value_t res = nullptr;
	if (type != Type::VOID) {
		res = jit_value_create(c.F, VM::get_jit_type(type));
	}

	jit_label_t label_else = jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;

	auto cond = condition->compile(c);
	condition->compile_end(c);

	if (condition->type.nature == Nature::POINTER) {
		auto cond_bool = c.insn_to_bool(cond);
		c.insn_delete_temporary(cond);
		jit_insn_branch_if_not(c.F, cond_bool.v, &label_else);
	} else {
		jit_insn_branch_if_not(c.F, cond.v, &label_else);
	}

	auto then_v = then->compile(c);
	then->compile_end(c);
	if (then_v.v) {
		jit_insn_store(c.F, res, then_v.v);
	}
	jit_insn_branch(c.F, &label_end);

	jit_insn_label(c.F, &label_else);

	if (elze != nullptr) {
		auto else_v = elze->compile(c);
		elze->compile_end(c);
		if (else_v.v) {
			jit_insn_store(c.F, res, else_v.v);
		}
	} else {
		if (type != Type::VOID) {
			jit_insn_store(c.F, res, c.new_null().v);
		}
	}

	jit_insn_label(c.F, &label_end);

	return {res, type};
}

Value* If::clone() const {
	auto iff = new If();
	iff->condition = condition->clone();
	iff->then = (Block*) then->clone();
	iff->elze = elze ? (Block*) elze->clone() : nullptr;
	iff->ternary = ternary;
	return iff;
}

}
