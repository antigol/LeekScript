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

// DONE 2
void Array::analyse_help(SemanticAnalyser* analyser)
{
	for (Value* ex : expressions) {
		ex->analyse(analyser);
	}
	type = Type::VEC;
}

void Array::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type element_type = type.element_type(0);
	do {
		type = Type(&RawType::VEC, { element_type });

		for (Value* ex : expressions) {
			ex->reanalyse(analyser, element_type);
			if (!Type::intersection(element_type, ex->type, &element_type)) {
				add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
			}
		}
	} while (element_type != type.element_type(0));
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
	element_type.make_it_pure(); // if empty
	type = Type(&RawType::VEC, { element_type });
	assert(type.is_pure() || !analyser->errors.empty());
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
