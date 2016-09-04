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
	preanalyse(analyser);

	if (!type.match_with_generic(req_type, &type)) {
		stringstream oss;
		print(oss, 0, false);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
	}
	type.make_it_complete();

	for (Value* ex : expressions) {
		ex->analyse(analyser, type.element_type(0));
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

void Array::preanalyse(SemanticAnalyser* analyser)
{
	constant = true;

	Type element_type = Type::UNKNOWN;

#if DEBUG > 0
	if (expressions.size() > 1) {
		cout << "#Array ";
	}
	bool first = true;
#endif

	for (Value* ex : expressions) {
		ex->preanalyse(analyser);
		constant = constant && ex->constant;
		element_type = Type::get_compatible_type(element_type, ex->type);
#if DEBUG > 0
		if (expressions.size() > 1) {
			if (first) first = false; else cout << " + ";
			cout << ex->type;
		}
#endif
	}
#if DEBUG > 0
	cout << " = " << element_type << endl;
#endif

	type = Type(&RawType::VEC, { element_type });
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
