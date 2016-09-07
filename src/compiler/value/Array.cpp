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

void Array::analyse_help(SemanticAnalyser* analyser)
{
	constant = true;

	Type element_type = Type::UNKNOWN;

	for (Value* ex : expressions) {
		ex->analyse(analyser);
		constant = constant && ex->constant;

//		cout << element_type << " + " << ex->type << " = ";
		if (!Type::intersection(element_type, ex->type, &element_type)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}
//		cout << element_type << endl;
	}

	for (Value* ex : expressions) {
		ex->reanalyse(analyser, element_type);
	}

	type = Type(&RawType::VEC, { element_type });
}

void Array::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type element_type = type.element_type(0);
	for (Value* ex : expressions) {
		ex->reanalyse(analyser, element_type);
		if (!Type::intersection(element_type, ex->type, &element_type)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}
	}
	type = Type(&RawType::VEC, { element_type });
}

void Array::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type element_type = type.element_type(0);
	for (Value* ex : expressions) {
		ex->finalize(analyser, element_type);
		element_type = ex->type;
	}
	type = Type(&RawType::VEC, { element_type });
	type.make_it_pure();
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
