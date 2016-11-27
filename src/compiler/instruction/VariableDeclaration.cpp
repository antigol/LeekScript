#include "../../compiler/instruction/VariableDeclaration.hpp"

#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "../value/Reference.hpp"
#include "../value/Function.hpp"

using namespace std;

namespace ls {

VariableDeclaration::VariableDeclaration() {
	global = false;
}

VariableDeclaration::~VariableDeclaration() {
	for (auto ex : expressions) {
		delete ex;
	}
}

void VariableDeclaration::print(ostream& os, int indent, bool debug) const {

	os << (global ? "global " : "let ");

	for (unsigned i = 0; i < variables.size(); ++i) {
		os << variables.at(i)->content;
		if (i < variables.size() - 1) {
			os << ", ";
		}
	}
	if (expressions.size() > 0) {
		os << " = ";
	}
	for (unsigned i = 0; i < expressions.size(); ++i) {
		expressions.at(i)->print(os, indent, debug);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
}

void VariableDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	type = Type::VOID;

	vars.clear();
	for (unsigned i = 0; i < variables.size(); ++i) {

		Token* var = variables[i];
		Value* value = nullptr;

		SemanticVar* v = analyser->add_var(var, Type::UNKNOWN, value, this);

		if (i < expressions.size()) {
			expressions[i]->analyse(analyser, Type::UNKNOWN);
			v->type = expressions[i]->type;
			v->value = expressions[i];
		}

		if (v->type == Type::VOID) {
			analyser->add_error({SemanticError::Type::CANT_ASSIGN_VOID, var->line, var->content});
		}

		vars.insert(pair<string, SemanticVar*>(var->content, v));
	}
}

Compiler::value VariableDeclaration::compile(Compiler& c) const {

	for (unsigned i = 0; i < variables.size(); ++i) {

		std::string name = variables[i]->content;
		SemanticVar* v = vars.at(name);

		if (i < expressions.size()) {

			Value* ex = expressions[i];

			if (Reference* ref = dynamic_cast<Reference*>(ex)) {
				jit_value_t val = c.get_var(ref->variable->content).value;
				c.add_var(name, val, v->type, true);
			} else {
				jit_value_t var = jit_value_create(c.F, VM::get_jit_type(v->type));
				c.add_var(name, var, Type::POINTER, false);

				if (Function* f = dynamic_cast<Function*>(ex)) {
					jit_insn_store(c.F, var, LS_CREATE_POINTER(c.F, (void*) f->ls_fun));
				}

				auto val = ex->compile(c);
				if (ex->type.must_manage_memory()) {
					val.v = VM::move_inc_obj(c.F, val.v);
				}
				c.set_var_type(name, ex->type);

				if (v->type == Type::GMP_INT) {
					jit_insn_store(c.F, var, VM::clone_gmp_int(c.F, val.v));
				} else {
					jit_insn_store(c.F, var, val.v);
				}
			}
		} else {

			jit_value_t var = jit_value_create(c.F, LS_POINTER);
			c.add_var(name, var, Type::NULLL, false);

			auto val = c.new_null();
			jit_insn_store(c.F, var, val.v);
		}
	}
	return {nullptr, Type::UNKNOWN};
}

}
