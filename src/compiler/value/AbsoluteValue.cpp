#include "../../compiler/value/AbsoluteValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;

namespace ls {

AbsoluteValue::AbsoluteValue() {
	expression = nullptr;
}

AbsoluteValue::~AbsoluteValue() {
	delete expression;
}

void AbsoluteValue::print(std::ostream& os, int indent, bool debug) const {
	os << "|";
	expression->print(os, indent, debug);
	os << "|";
}

unsigned AbsoluteValue::line() const {
	return 0;
}

// DONE 2
void AbsoluteValue::analyse_help(SemanticAnalyser* analyser)
{
	expression->analyse(analyser);
	if (!Type::intersection(expression->type, Type::VAR, &expression->type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type = Type::VAR;
}

void AbsoluteValue::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	expression->reanalyse(analyser, Type::UNKNOWN);
}

void AbsoluteValue::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	expression->finalize(analyser, Type::UNKNOWN);
}

jit_value_t AbsoluteValue::compile(Compiler& c) const
{
	jit_value_t ex = expression->compile(c);
	return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) LSVar::ls_abso, { ex });
}

}
