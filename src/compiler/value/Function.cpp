#include "Function.hpp"

#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Program.hpp"
#include <jit/jit-dump.h>
using namespace std;

namespace ls {

Function::Function() {
	body = nullptr;
	parent = nullptr;
	pos = 0;
	constant = true;
	function_added = false;
}

Function::~Function() {
	delete body;
	for (auto value : defaultValues) {
		delete value;
	}
	if (ls_fun != nullptr) {
		delete ls_fun;
	}
}

void Function::addArgument(Token* name, bool reference, Value* defaultValue) {
	arguments.push_back(name);
	references.push_back(reference);
	defaultValues.push_back(defaultValue);
}

void Function::print(std::ostream& os, int indent, bool debug) const {

	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c]->name << " " << captures[c]->type;
		}
		os << "] ";
	}

	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		if (i > 0) os << ", ";
//		if (references.at(i)) {
//			os << "@";
//		}
		os << arguments.at(i)->content;
//
//		if ((Value*)defaultValues.at(i) != nullptr) {
//			os << " = ";
//			defaultValues.at(i)->print(os);
//		}
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

void Function::analyse(SemanticAnalyser* analyser, const Type& req_type) {

//	cout << "Function::analyse req_type " << req_type << endl;

	captures.clear();

	parent = analyser->current_function();

	if (!function_added) {
		analyser->add_function(this);
		function_added = true;
	}

	type = Type::FUNCTION_P;

	for (unsigned int i = 0; i < arguments.size(); ++i) {
		type.setArgumentType(i, Type::UNKNOWN);
	}

	for (unsigned int i = 0; i < req_type.getArgumentTypes().size(); ++i) {
		type.setArgumentType(i, req_type.getArgumentType(i));
	}

	analyse_body(analyser, req_type.getReturnType());

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	if (ls_fun == nullptr) {
		ls_fun = new LSFunction(nullptr);
		ls_fun->return_type = LSNull::get();
	}
	update_function_args(analyser);
	type.nature = Nature::POINTER;

//	cout << "Function type: " << type << endl;
}

bool Function::will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types) {

//	cout << "Function::will_take " << arg_type << " at " << pos << endl;

	bool changed = type.will_take(arg_types);

	if (changed) {
		analyse_body(analyser, type.getReturnType());
	}
//	cout << "Function::will_take type after " << type << endl;
	update_function_args(analyser);

	return changed;
}

void Function::must_return(SemanticAnalyser* analyser, const Type& ret_type) {

//	cout << "Function::must_return : " << ret_type << endl;

	type.setReturnType(ret_type);

	analyse_body(analyser, ret_type);
}

void Function::analyse_body(SemanticAnalyser* analyser, const Type& req_type) {

	analyser->enter_function(this);

	for (unsigned i = 0; i < arguments.size(); ++i) {
//		cout << "arg " << i << " type : " << type.getArgumentType(i) << endl;
		analyser->add_parameter(arguments[i], type.getArgumentType(i));
	}

	type.setReturnType(Type::UNKNOWN);
	body->analyse(analyser, req_type);
	if (type.return_types.size() > 1) { // the body contains return instruction
		Type return_type = body->type == Type::VOID ? Type::UNKNOWN : body->type;
		for (size_t i = 1; i < type.return_types.size(); ++i) {
			return_type = Type::get_compatible_type(return_type, type.return_types[i]);
		}
		type.return_types.clear();
		type.setReturnType(return_type);
		body->analyse(analyser, return_type); // second pass
	} else {
		type.setReturnType(body->type);
	}

	if (type.getReturnType() == Type::GMP_INT) {
		type.setReturnType(Type::GMP_INT_TMP);
	}

	vars = analyser->get_local_vars();
	analyser->leave_function();

//	cout << "function analyse body : " << type << endl;
}

int Function::capture(SemanticVar* var) {

	if (std::find(captures.begin(), captures.end(), var) == captures.end()) {

//		cout << "Function::capture " << var->name << endl;

		captures.push_back(var);

		if (var->function != parent) {
			parent->capture(var);
		}
	}
	type.nature = Nature::POINTER;
	return std::distance(captures.begin(), std::find(captures.begin(), captures.end(), var));
}

void Function::update_function_args(SemanticAnalyser* analyser) {
	ls_fun->args.clear();
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto& clazz = type.getArgumentType(i).raw_type->getClass();
		LSClass* arg_class = (LSClass*) analyser->program->system_vars[clazz];
		if (arg_class != nullptr) {
			ls_fun->args.push_back((LSValue*) arg_class);
		} else {
			ls_fun->args.push_back(LSNull::get());
		}
	}
	auto return_class_name = type.getReturnType().raw_type->getClass();
	LSClass* return_class = (LSClass*) analyser->program->system_vars[return_class_name];
	if (return_class != nullptr) {
		ls_fun->return_type = (LSValue*) return_class;
	} else {
		ls_fun->return_type = LSNull::get();
	}
}

Compiler::value Function::compile(Compiler& c) const {

//	cout << "Function::compile : " << type << endl;

	jit_context_t context = jit_context_create();
	jit_context_build_start(context);

	unsigned arg_count = arguments.size() + 1;
	vector<jit_type_t> params = {LS_POINTER}; // first arg is the function pointer

	for (unsigned i = 0; i < arg_count - 1; ++i) {
		Type t = Type::INTEGER;
		if (i < type.getArgumentTypes().size()) {
			t = type.getArgumentType(i);
		}
		params.push_back(VM::get_jit_type(t));
	}

	jit_type_t return_type = VM::get_jit_type(type.getReturnType());

	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arg_count, 0);
	jit_function_t function = jit_function_create(context, signature);

//	cout << "return type : " << type.getReturnType() << endl;

	c.enter_function(function);

	// Execute function
	auto res = body->compile(c);

	// Return
	jit_insn_return(function, res.v);

	jit_insn_rethrow_unhandled(function);

	//jit_dump_function(fopen("f_uncompiled", "w"), function, "f");

	jit_function_compile(function);
	jit_context_build_end(context);

	//jit_dump_function(fopen("f_compiled", "w"), function, "f");

	void* f = jit_function_to_closure(function);

//	cout << "function : " << f << endl;

	c.leave_function();

	// Create a function : 1 op
	VM::inc_ops(c.F, 1);

	if (type.nature == Nature::POINTER) {
		ls_fun->function = f;
		jit_value_t jit_fun = LS_CREATE_POINTER(c.F, ls_fun);
		for (const auto& cap : captures) {
			jit_value_t jit_cap;
			if (cap->scope == VarScope::LOCAL) {
//				std::cout << "capture local" << std::endl;
				jit_cap = c.get_var(cap->name).value;
			} else {
//				std::cout << "capture parameter" << std::endl;
				jit_cap = jit_value_get_param(c.F, 1 + cap->index);
			}
			if (cap->type.nature != Nature::POINTER) {
				jit_cap = VM::value_to_pointer(c.F, jit_cap, cap->type);
			}
			VM::function_add_capture(c.F, jit_fun, jit_cap);
		}
		return {jit_fun, type};
	} else {
		return {LS_CREATE_POINTER(c.F, f), type};
	}
}

}
