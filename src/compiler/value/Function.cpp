#include "Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

Function::Function() {
	body = nullptr;
//	parent = nullptr;
//	pos = 0;
	constant = true;
	returnType = nullptr;
//	function_added = false;
}

Function::~Function() {
	delete body;
	for (auto value : defaultValues) {
		delete value;
	}
	for (auto value : typeNames) {
		delete value;
	}
	delete returnType;
}

void Function::addArgument(Token* name, bool reference, TypeName* typeName, Value* defaultValue) {
	arguments.push_back(name);
	references.push_back(reference);
	typeNames.push_back(typeName);
	defaultValues.push_back(defaultValue);
}

void Function::print(std::ostream& os, int indent, bool debug) const {

	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c]->name;
		}
		os << "] ";
	}

	os << "(";
	for (size_t i = 0; i < arguments.size(); ++i) {
		if (i > 0) os << ", ";
//		if (references.at(i)) {
//			os << "@";
//		}
		os << arguments[i]->content;
		if (i < typeNames.size() && typeNames[i]) {
			os << ": ";
			typeNames[i]->print(os);
		}

//
//		if ((Value*)defaultValues.at(i) != nullptr) {
//			os << " = ";
//			defaultValues.at(i)->print(os);
//		}
	}

	if (returnType) {
		os << ")→ ";
		returnType->print(os);
	}

	os << ") → ";
	body->print(os, indent, debug);

	if (debug) {
		os << " " << type;
	}
}

unsigned Function::line() const {
	return 0;
}

void Function::preanalyse(SemanticAnalyser* analyser)
{
	type = Type::FUNCTION;

	for (size_t i = 0; i < arguments.size(); ++i) {
		if (i < typeNames.size() && typeNames[i]) {
			type.set_argument_type(i, typeNames[i]->getInternalType(analyser));
		} else {
			type.set_argument_type(i, Type::UNKNOWN);
		}
	}

	Type req_return_type = Type::UNKNOWN;
	if (returnType) {
		req_return_type = returnType->getInternalType(analyser);
	}

	analyser->enter_function(this);
	for (size_t i = 0; i < arguments.size(); ++i) {
		analyser->add_parameter(arguments[i], type.argument_type(i));
	}

	type.set_return_type(Type::UNKNOWN);

	// The following call will modify
	// type.return_types (return instruction)
	// type.arguments_type (parameters)
	body->preanalyse(analyser);

	// Compute intersection of returns types
	Type return_type = req_return_type;
	type.return_types[0] = body->type;
	for (size_t i = 0; i < type.return_types.size(); ++i) {
		if (type.return_types[i] == Type::UNREACHABLE) continue;
		if (!Type::intersection(return_type, type.return_types[i], &return_type)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}
	}
	type.return_types.clear();
	type.set_return_type(return_type);

	analyser->leave_function();
}

void Function::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();

	analyser->enter_function(this);
	for (size_t i = 0; i < arguments.size(); ++i) {
		analyser->add_parameter(arguments[i], type.argument_type(i));
	}

	body->analyse(analyser, type.return_type());

	analyser->leave_function();
}

void Function::capture(SemanticVar* var) {

//	if (std::find(captures.begin(), captures.end(), var) == captures.end()) {
//		captures.push_back(var);

//		if (var->function != parent) {
//			parent->capture(var);
//		}
//	}
}

jit_value_t Function::compile(Compiler& c) const {

	jit_context_t context = jit_context_create();
	jit_context_build_start(context);

	vector<jit_type_t> params;
	for (size_t i = 0; i < arguments.size(); ++i) {
		params.push_back(type.argument_type(i).jit_type());
	}

	jit_type_t return_type = type.return_type().jit_type();

	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arguments.size(), 0);
	jit_function_t function = jit_function_create(context, signature);

	c.enter_function(function);

	// Execute function
	jit_value_t res = body->compile(c);

	// Return
	jit_insn_return(function, res);

	jit_insn_rethrow_unhandled(function);

	jit_function_compile(function);
	jit_context_build_end(context);

	void* f = jit_function_to_closure(function);

	c.leave_function();

	// Create a function : 1 op
	VM::inc_ops(c.F, 1);

	return VM::create_ptr(c.F, f);
}

}
