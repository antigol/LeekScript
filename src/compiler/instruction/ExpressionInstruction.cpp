#include "ExpressionInstruction.hpp"

using namespace std;

namespace ls {

ExpressionInstruction::ExpressionInstruction(Value* value) {
	this->value = value;
}

ExpressionInstruction::~ExpressionInstruction() {
	delete this->value;
}

void ExpressionInstruction::print(ostream& os, int indent, bool debug) const {
	value->print(os, indent, debug);
}

unsigned ExpressionInstruction::line() const
{
	return 0;
}

void ExpressionInstruction::preanalyse(SemanticAnalyser* analyser)
{
	value->preanalyse(analyser);
	type = Type({ value->type, Type::VOID});
}

void ExpressionInstruction::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (type != Type::VOID) {
		value->will_require(analyser, type);
		type = Type({ value->type, Type::VOID});
	}
}

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (type == Type::VOID) {
		value->analyse(analyser, Type::UNKNOWN);
		if (value->type == Type::UNREACHABLE) type = Type::UNREACHABLE;
	} else {
		value->analyse(analyser, type);
		type = value->type;
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

jit_value_t ExpressionInstruction::compile(Compiler& c) const {

	jit_value_t v = value->compile(c);

	if (type == Type::VOID) {
		if (value->type.must_manage_memory()) {
			VM::delete_temporary(c.F, v);
		}
		return nullptr;
	} else {
		return v;
	}
}

}
