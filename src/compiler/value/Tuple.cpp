#include "Tuple.hpp"

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

void Tuple::preanalyse(SemanticAnalyser* analyser)
{
	type = Type::TUPLE;

	for (Value* val : elements) {
		val->preanalyse(analyser);
		type.elements_types.push_back(val->type);
	}
}

void Tuple::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	for (size_t i = 0; i < elements.size(); ++i) {
		elements[i]->will_require(analyser, type.element_type(i));
	}
}

void Tuple::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();

	for (size_t i = 0; i < elements.size(); ++i) {
		elements[i]->analyse(analyser, type.element_type(i));
	}
}

jit_value_t Tuple::compile(Compiler& c) const
{
	jit_value_t val = jit_value_create(c.F, type.jit_type());
	jit_value_t ptr = jit_insn_address_of(c.F, val);
	for (size_t i = 0; i < elements.size(); ++i) {
		jit_value_t el = elements[i]->compile(c);
		jit_insn_store_relative(c.F, ptr, jit_type_get_offset(type.jit_type(), i), el); // TODO : increment lsvalue
	}
	return val;
}


}
