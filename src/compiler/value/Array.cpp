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

void Array::analyse(SemanticAnalyser* analyser, const Type& req_type) {


	constant = true;
	type = Type::VEC;

	if (req_type != Type::UNKNOWN && req_type.raw_type != RawType::VEC) {
		std::ostringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}

	Type element_type = req_type.getElementType();

	for (size_t i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		ex->analyse(analyser, req_type.getElementType());

		if (ex->constant == false) {
			constant = false;
		}
		element_type = Type::get_compatible_type(element_type, ex->type);
		if (element_type == Type::VOID) {
			std::ostringstream oss;
			expressions[i]->print(oss);
			analyser->add_error({ SemanticException::TYPE_MISMATCH, expressions[i]->line(), oss.str() });
			break;
		}
	}

	// Re-analyze expressions with the supported type
	for (size_t i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		ex->analyse(analyser, element_type);
		if (ex->type != element_type) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}

	type.setElementType(0, element_type);

}

jit_value_t Array::compile(Compiler& c) const {

	jit_value_t array = VM::create_vec(c.F, type.getElementType(), expressions.size());

	for (Value* val : expressions) {

		jit_value_t v = val->compile(c);
		VM::push_move_vec(c.F, type.getElementType(), array, v);
	}

	// size of the array + 1 operations
	VM::inc_ops(c.F, expressions.size() + 1);

	return array;
}

}
