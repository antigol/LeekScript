#include "Tuple.hpp"
#include "../jit/jit_general.hpp"

using namespace std;

namespace ls {

Tuple::Tuple()
{

}

void Tuple::print(std::ostream& os, int indent, bool debug) const
{
	os << "(";
	for (size_t i = 0; i < elements.size(); ++i) {
		if (i > 0) os << ", ";
		elements[i]->print(os, indent + 1, debug);
	}
	if (elements.size() == 1) os << ",";
	os << ")";
	if (debug) os << " " << type;
}

unsigned Tuple::line() const
{
	return 0;
}

// DONE 2
void Tuple::analyse_help(SemanticAnalyser* analyser)
{
	type = Type::TUPLE;

	for (Value* val : elements) {
		val->analyse(analyser);
		type.elements_types.push_back(val->type);
	}
}

void Tuple::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	for (size_t i = 0; i < elements.size(); ++i) {
		Value* e = elements[i];
		e->reanalyse(analyser, type.element_type(i));
		type.set_element_type(i, e->type);
	}
}

void Tuple::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	for (size_t i = 0; i < elements.size(); ++i) {
		Value* e = elements[i];
		e->finalize(analyser, type.element_type(i));
		type.set_element_type(i, e->type);
	}
}

jit_value_t Tuple::compile(Compiler& c) const
{
	jit_type_t jit_type = type.jit_type();
	jit_value_t val = jit_value_create(c.F, jit_type);
	jit_value_t ptr = jit_insn_address_of(c.F, val);
	for (size_t i = 0; i < elements.size(); ++i) {
		jit_value_t el = elements[i]->compile(c);
		el = jit_general::move(c.F, el, type.element_type(i));
		jit_insn_store_relative(c.F, ptr, jit_type_get_offset(jit_type, i), el);
	}
	jit_type_free(jit_type);
	return val;
}


}
