#include "../../compiler/instruction/ExpressionInstruction.hpp"

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

void ExpressionInstruction::analyse(SemanticAnalyser* analyser, const Type& req_type) {
	if (req_type == Type::VOID) {
		value->analyse(analyser, Type::UNKNOWN);
		type = Type::VOID;
	} else {
		value->analyse(analyser, req_type);
		type = value->type;
	}
	assert(type.is_complete() || !analyser->errors.empty());
}

void ExpressionInstruction::preanalyse(SemanticAnalyser* analyser)
{
	value->preanalyse(analyser);
	type = value->type;
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
