#include "Function.hpp"
#include "../jit/jit_general.hpp"

using namespace std;

namespace ls {

Function::Function() {
	body = nullptr;
//	parent = nullptr;
//	pos = 0;
	constant = true;
	returnType = nullptr;
	self_var = nullptr;
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
		if (debug) {
			os << " " << type.argument_type(i);
		}
		if (i < typeNames.size() && typeNames[i]) {
			os << ": ";
			typeNames[i]->print(os);
		}

//		if ((Value*)defaultValues.at(i) != nullptr) {
//			os << " = ";
//			defaultValues.at(i)->print(os);
//		}
	}

	os << ") ";
	if (debug) {
		os << type.return_type();
	}
	if (returnType) {
		os << " → ";
		returnType->print(os);
	}
	body->print(os, indent, debug);
}

unsigned Function::line() const {
	return 0;
}

// DONE 2
void Function::analyse_help(SemanticAnalyser* analyser)
{
	type = Type::FUNCTION;

	for (size_t i = 0; i < arguments.size(); ++i) {
		if (i < typeNames.size() && typeNames[i]) {
			Type t = typeNames[i]->getInternalType();
			if (t == Type::UNKNOWN) add_error(analyser, SemanticException::UNKNOWN_TYPE);
			type.set_argument_type(i, t);
		} else {
			type.set_argument_type(i, Type::UNKNOWN);
		}
	}

	if (returnType) {
		Type t = returnType->getInternalType();
		if (t == Type::UNKNOWN) add_error(analyser, SemanticException::UNKNOWN_TYPE);
		type.set_return_type(t);
	} else {
		type.set_return_type(Type::UNKNOWN);
	}

	analyser->enter_function(this);
	if (!self_name.empty()) {
		self_var = analyser->add_var(self_name, type, this);
	}

	arguments_vars.clear();
	for (size_t i = 0; i < arguments.size(); ++i) {
		arguments_vars.push_back(analyser->add_parameter(arguments[i], type.argument_type(i), this));
	}

	body->function = this; // just in case
	body->analyse(analyser);

	analyser->leave_function();
}

void Function::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	DBOUT(cout << "FUN wr " << type << " + " << req_type << " + arguments types = ");

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (self_var && !Type::intersection(type, self_var->type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	for (size_t i = 0; i < arguments.size(); ++i) {
		Type arg_type = type.argument_type(i);
		if (!Type::intersection(arg_type, arguments_vars[i]->type, &arg_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		arguments_vars[i]->type = arg_type;
		type.set_argument_type(i, arg_type);
	}
	if (self_var) self_var->type = type;

	DBOUT(cout << type << endl);

	// tip! Block/Return will read/write return_type
	body->reanalyse(analyser, Type::UNKNOWN);
}

void Function::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (self_var && !Type::intersection(type, self_var->type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	// make the arguments pure
	for (size_t i = 0; i < arguments.size(); ++i) {
		Type arg_type = type.argument_type(i);
		if (!Type::intersection(arg_type, arguments_vars[i]->type, &arg_type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		arg_type.make_it_pure();
		arguments_vars[i]->type = arg_type;
		type.set_argument_type(i, arg_type);
	}
	if (self_var) self_var->type = type;

	// body makes the return_type pure
	body->finalize(analyser, Type::UNKNOWN);

	assert(type.is_pure() || !analyser->errors.empty());
}

void Function::capture(SemanticVar* )
{
//	if (std::find(captures.begin(), captures.end(), var) == captures.end()) {
//		captures.push_back(var);

//		if (var->function != parent) {
//			parent->capture(var);
//		}
//	}
}

jit_value_t Function::compile(Compiler& c) const
{
	jit_context_t context = jit_context_create();
	jit_context_build_start(context);

	vector<jit_type_t> params;
	for (size_t i = 0; i < arguments.size(); ++i) {
		params.push_back(type.argument_type(i).jit_type());
	}

	jit_type_t return_type = type.return_type().jit_type();

	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arguments.size(), 0);
	jit_function_t G = jit_function_create(context, signature);

	c.enter_function(G);

	// Own the arguments
	for (size_t i = 0; i < arguments.size(); ++i) {
		if (type.arguments_types[i].must_manage_memory()) {
			jit_value_t p = jit_value_get_param(c.F, i);
			jit_value_t q = jit_general::move_inc(c.F, p, type.argument_type(i));
			jit_insn_store(c.F, p, q);
		}
	}

	c.add_var(self_name, nullptr, type, true);

	// Execute function
	jit_value_t res = body->compile(c);

	// Delete owned arguments
	for (size_t i = 0; i < arguments.size(); ++i) {
		jit_general::delete_ref(c.F, jit_value_get_param(c.F, i), type.argument_type(i));
	}

	// Return
	jit_insn_return(G, res);

	jit_insn_rethrow_unhandled(G);

	jit_function_compile(G);
	jit_context_build_end(context);

	void* f = jit_function_to_closure(G);

	c.leave_function();

	// Create a function : 1 op
	VM::inc_ops(c.F, 1);

	return jit_general::constant_ptr(c.F, f);
}

}
