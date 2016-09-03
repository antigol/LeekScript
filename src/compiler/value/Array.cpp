#include "Array.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include <math.h>

using namespace std;

namespace ls {

Array::Array() {
}

Array::~Array() {
	for (auto ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug) const {
	os << "[";
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->print(os, indent, debug);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
	os << "]";

	if (debug) {
		os << " " << type;
	}
}

unsigned Array::line() const {
	return 0;
}

void Array::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	constant = true;
	type = Type::VEC;

	if (req_type != Type::UNKNOWN && req_type.raw_type != RawType::VEC) {
		std::ostringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}

	Type element_type = req_type.element_type(0);

	for (Value* ex : expressions) {
		ex->preanalyse(analyser, req_type.element_type(0));

		constant = constant && ex->constant;
		element_type = Type::get_compatible_type(element_type, ex->type);
		if (element_type == Type::VOID) {
			std::ostringstream oss;
			ex->print(oss);
			analyser->add_error({ SemanticException::TYPE_MISMATCH, ex->line(), oss.str() });
			break;
		}
	}

	if (element_type == Type::UNKNOWN) {
		// empty array
		element_type = Type::VAR;
	}

	// Re-analyze expressions with the supported type
	for (Value* ex : expressions) {
		ex->analyse(analyser, element_type);
		if (ex->type != element_type) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}

	type.set_element_type(0, element_type);
	assert(type.is_complete());
}

void Array::preanalyse(SemanticAnalyser* analyser, const Type& req_type)
{
	constant = true;
	type = Type::VEC;

	Type element_type = req_type.element_type(0);

	for (Value* ex : expressions) {
		ex->preanalyse(analyser, req_type.element_type(0));
		constant = constant && ex->constant;
		element_type = Type::get_compatible_type(element_type, ex->type);
	}

	// Re-analyze expressions with the supported type
	for (Value* ex : expressions) {
		ex->preanalyse(analyser, element_type);
	}

	type.set_element_type(0, element_type);
}

jit_value_t Array::compile(Compiler& c) const {

	jit_value_t array = VM::create_vec(c.F, type.element_type(0), expressions.size());

	for (Value* val : expressions) {
		jit_value_t v = val->compile(c);
		VM::push_move_inc_vec(c.F, type.element_type(0), array, v);
	}

	// size of the array + 1 operations
	VM::inc_ops(c.F, expressions.size() + 1);

	return array;
}

}
