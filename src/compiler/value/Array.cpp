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

void Array::preanalyse(SemanticAnalyser* analyser)
{
	constant = true;

	Type element_type = Type::UNKNOWN;

#if DEBUG > 0
	cout << "#Array ";
	print(cout, 0, false);
	cout << "  ";
	bool first = true;
#endif

	for (Value* ex : expressions) {
		ex->preanalyse(analyser);
		constant = constant && ex->constant;

		if (!Type::intersection(element_type, ex->type, &element_type)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}

#if DEBUG > 0
		if (first) first = false; else cout << " + ";
		cout << ex->type;
#endif
	}
#if DEBUG > 0
	cout << " = " << element_type << endl;
#endif

	for (Value* ex : expressions) {
		ex->will_require(analyser, element_type);
	}

	type = Type(&RawType::VEC, { element_type });
}

void Array::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();

	for (Value* ex : expressions) {
		ex->analyse(analyser, type.element_type(0));
	}
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
