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

	Type element_type = req_type.getElementType();

	for (size_t i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		ex->analyse(analyser, req_type.getElementType());

		if (ex->constant == false) {
			constant = false;
		}
		element_type = Type::get_compatible_type(element_type, ex->type);
		if (element_type == Type::VOID) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
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

void Array::elements_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {
/*
//	cout << "Array::elements_will_take " << type << " at " << pos << endl;

	for (size_t i = 0; i < expressions.size(); ++i) {

		Array* arr = dynamic_cast<Array*>(expressions[i]);
		if (arr != nullptr && level > 0) {
			arr->elements_will_take(analyser, arg_types, level - 1);
		} else {
			expressions[i]->will_take(analyser, arg_types);
		}
	}

	// Computation of the new array type
	Type element_type;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = Type::get_compatible_type(element_type, ex->type);
		}
	}
	this->type.setElementType(element_type);

//	cout << "Array::elements_will_take type after " << this->type << endl;
*/
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
