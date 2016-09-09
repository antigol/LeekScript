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

// DONE 2
void ExpressionInstruction::analyse_help(SemanticAnalyser* analyser)
{
	value->analyse(analyser);
	if (value->type == Type::UNREACHABLE) type = Type::UNREACHABLE;
	else type = Type({ value->type, Type::VOID});
}

void ExpressionInstruction::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	DBOUT(cout << "EI wr " << type << " + " << req_type << " = ");

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	DBOUT(cout << type << " => ");
	if (Type::intersection(type, Type::VOID)) {
		DBOUT(cout << "ask UNKNOWN" << endl);
		value->reanalyse(analyser, Type::UNKNOWN); // because of the void we cannot require anything
		if (value->type == Type::UNREACHABLE) {
			type = Type::UNREACHABLE;
		} else {
			if (!Type::intersection(type, Type({ Type::VOID, value->type }), &type)) {
				add_error(analyser, SemanticException::TYPE_MISMATCH);
			}
		}
	} else {
		DBOUT(cout << "ask type" << endl);
		value->reanalyse(analyser, type);
		// tip! If value returns UNREACHABLE type will naturaly become UNREACHABLE via the intersection
		if (!Type::intersection(type, value->type, &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}
}

void ExpressionInstruction::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (type == Type::UNREACHABLE) {
		value->finalize(analyser, Type::UNKNOWN);
	} else {
		if (!Type::intersection(type, req_type, &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

		if (type == Type::VOID) {
			value->finalize(analyser, Type::UNKNOWN);
			if (value->type == Type::UNREACHABLE) type = Type::UNREACHABLE;
		} else {
			value->finalize(analyser, type);
			type = value->type;
		}
	}
	assert(type.is_pure() || !analyser->errors.empty());
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
