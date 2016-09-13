#include "Set.hpp"
#include "../jit/jit_set.hpp"

using namespace std;

namespace ls {

Set::Set() {
}

Set::~Set() {
	for (auto ex : expressions) delete ex;
}

void Set::print(ostream& os, int indent, bool debug) const {
	os << "<" << endl;
	for (size_t i = 0; i < expressions.size(); ++i) {
		os << tabs(indent + 1);
		expressions[i]->print(os, indent + 1, debug);
		os << "\n";
	}
	os << tabs(indent) << ">";
	if (debug) os << " " << type;
}

unsigned Set::line() const {
	return 0;
}

// DONE 2
void Set::analyse_help(SemanticAnalyser* analyser)
{
	for (Value* ex : expressions) {
		ex->analyse(analyser);
	}
	type = Type::SET;
}

void Set::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type element_type = type.element_type(0);
	do {
		type = Type(&RawType::SET, { element_type });

		for (Value* ex : expressions) {
			ex->reanalyse(analyser, element_type);
			if (!Type::intersection(element_type, ex->type, &element_type)) {
				add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
			}
		}
	} while (element_type != type.element_type(0));
}

void Set::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
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
	type = Type(&RawType::SET, { element_type });
	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t Set::compile(Compiler& c) const
{
	jit_value_t set = jit_set::create(c.F, type.elements_types[0]);

	for (Value* val : expressions) {
		jit_value_t v = val->compile(c);
		jit_set::insert_move_inc(c.F, type.elements_types[0], set, v);
	}

	// size of the set + 1 operations
	VM::inc_ops(c.F, expressions.size() + 1);

	return set;
}


}
