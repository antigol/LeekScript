#include "Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../instruction/ExpressionInstruction.hpp"
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
	if (default_version != nullptr) {
		if (default_version->function != nullptr) {
			delete default_version->function;
		}
		delete default_version;
		default_version = nullptr;
	}
	for (const auto& version : versions) {
		delete version.second->function;
		delete version.second->body;
		delete version.second;
	}
}

void Function::addArgument(Token* name, bool reference, Value* defaultValue) {
	arguments.push_back(std::unique_ptr<Token> { name });
	references.push_back(reference);
	defaultValues.push_back(defaultValue);
}

Type Function::getReturnType() {
	if (type.getReturnType() == Type::UNKNOWN) {
		if (placeholder_type == Type::UNKNOWN) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		return placeholder_type;
	} else {
		return type.getReturnType();
	}
}

void Function::print_version(std::ostream& os, int indent, bool debug, const Version* version) const {
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
		if (references.at(i)) {
			os << "@";
		}
		os << arguments.at(i)->content;
		if (debug)
			os << " " << version->type.getArgumentType(i);

		if (defaultValues.at(i) != nullptr) {
			os << " = ";
			defaultValues.at(i)->print(os);
		}
	}
	os << ") → ";
	version->body->print(os, indent, debug);

	if (versions.size()) {
		os << " [" << versions.size() << " versions]";
		for (auto& v : versions) {
			os << v.first << " ";
		}
		os << std::endl;
	}

	if (debug) {
		//os << " " << type;
	}
}

void Function::print(std::ostream& os, int indent, bool debug) const {
	if (has_version || versions.size() == 1) {
		print_version(os, indent, debug, versions.begin()->second);
	} else {
		print_version(os, indent, debug, default_version);
	}
}

Location Function::location() const {
	return body->location();
}

/*
 * When returing a function, compile a default version with all parameters
 * as pointers, when the function will be in the nature, there will be no
 * problem. Inside a block, it's possible to compile a more specific version
 * of a function in this block to speed up things

let f = x -> x + '!'
let g2 = f('hello')   // default version with pointer
let g1 = f(10)		   // version with [x = int]

let f = x -> y -> x + y
let g = f('hello ')	// default version of f
let h1 = g(10)			// here we act like we don't know where does 'g' come from, and take the default version with pointer
let h2 = g('world')	// same version, pointers

let f = function(a) {
	let g = x -> x + a
	g('hello')	// default version with pointer
	g(12)		// version with [x = int]
	return g	// here, g will go outside his parent, so we take the full-pointer version, regardless of the version of function f. So f can be recompile multiple times, it will not affect the arguments of g and its return type.
}
let r1 = f(' !')		// default version
let r2 = f(12)			// version with number, recompiler f with a [a = int] version, it will not modify the signature of g
r2(12)
r2('hello')
 */
void Function::analyse(SemanticAnalyser* analyser, const Type& req_type) {

//	cout << "Function::analyse req_type " << req_type << endl;

	parent = analyser->current_function();

	if (!function_added) {
		analyser->add_function(this);
		function_added = true;
	}

	type = Type::FUNCTION_P;

	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto argument_type = Type::UNKNOWN;
		if (defaultValues[i] != nullptr) {
			defaultValues[i]->analyse(analyser, Type::UNKNOWN);
		}
		type.setArgumentType(i, argument_type, defaultValues[i] != nullptr);
	}

	for (unsigned int i = 0; i < req_type.getArgumentTypes().size(); ++i) {
		type.setArgumentType(i, Type::POINTER);
	}

	if (!default_version) {
		default_version = new Function::Version();
		current_version = default_version;
		default_version->body = body;
		default_version->function = new LSFunction<LSValue*>(nullptr);
		default_version->function->refs = 1;
		default_version->function->native = true;
	}

	auto return_type = req_type.getReturnType();
	analyse_body(analyser, type.getArgumentTypes(), default_version, return_type);

	type = default_version->type;

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	update_function_args(analyser);
	type.nature = Nature::POINTER;

	types = type;

//	cout << "Function type: " << type << endl;
}

bool Function::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {

	// cout << "Function::will_take " << args << " level " << level << endl;

	if (level == 1) {
		if (versions.find(args) == versions.end()) {
			auto version = new Function::Version();
			version->body = (Block*) body->clone();
			version->function = new LSFunction<LSValue*>(nullptr);
			version->function->refs = 1;
			version->function->native = true;
			versions.insert({args, version});

			analyse_body(analyser, args, version, Type::UNKNOWN);

			// std::cout << "version type : " << version->type << std::endl;
			update_function_args(analyser);
			return true;
		}
		return false;
	} else {
		if (auto ei = dynamic_cast<ExpressionInstruction*>(body->instructions[0])) {
			if (auto f = dynamic_cast<Function*>(ei->value)) {

				analyser->enter_function(this);
				for (unsigned i = 0; i < arguments.size(); ++i) {
					analyser->add_parameter(arguments[i].get(), type.getArgumentType(i));
				}

				f->will_take(analyser, args, level - 1);

				analyser->leave_function();

				// analyse_body(analyser, args, Type::UNKNOWN);
			}
		}
	}
	return false;
}

void Function::analyse_body(SemanticAnalyser* analyser, std::vector<Type> args, Version* version, const Type& req_type) {

	// std::cout << "Function::analyse_body(" << args << ", " << req_type << ")" << std::endl;

	captures.clear();

	analyser->enter_function(this);
	current_version = version;

	for (unsigned i = 0; i < arguments.size(); ++i) {
		Type type = i < args.size() ? args.at(i) : (i < defaultValues.size() ? defaultValues.at(i)->type : Type::UNKNOWN);
		analyser->add_parameter(arguments[i].get(), type);
	}

	version->type = Type::FUNCTION_P;
	version->type.arguments_types = args;
	// Set default arguments information
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		bool has_default = i < defaultValues.size() && defaultValues[i] != nullptr;
		version->type.setArgumentType(i, version->type.getArgumentType(i), has_default);
	}
	version->type.setReturnType(type.getReturnType());
	version->body->analyse(analyser, req_type);

	TypeList return_types;
	// Ignore recursive types
	for (const auto& t : version->body->types) {
		if (placeholder_type != Type::UNKNOWN and t == placeholder_type) {
			continue;
		}
		return_types.add(t);
	}

	if (version->body->types.size() >= 2) {
		// The body had multiple types, compute a compatible type and re-analyse it
		Type return_type = return_types[0];
		for (const auto& t : return_types) {
			return_type = Type::get_compatible_type(return_type, t);
		}
		version->type.return_types.clear();
		version->type.setReturnType(return_type);
		version->body->analyse(analyser, return_type); // second pass
	} else {
		if (return_types.size() > 0) {
			version->type.setReturnType(return_types[0]);
		} else {
			version->type.setReturnType(version->body->type);
		}
	}

	if (version->type.getReturnType() == Type::MPZ) {
		version->type.setReturnType(Type::MPZ_TMP);
	}

	vars = analyser->get_local_vars();
	analyser->leave_function();

	// cout << "function analysed body : " << version->type << endl;
}

int Function::capture(std::shared_ptr<SemanticVar> var) {
	for (size_t i = 0; i < captures.size(); ++i) {
		if (captures[i]->name == var->name)
			return i;
	}
	// var = std::make_shared<SemanticVar>(*var);
	captures.push_back(var);

	if (var->function != parent) {
		auto new_var = std::make_shared<SemanticVar>(*var);
		new_var->index = parent->capture(new_var);
		var->scope = VarScope::CAPTURE;
		var->parent_index = new_var->index;
	}
	type.nature = Nature::POINTER;
	return captures.size() - 1;
}

void Function::update_function_args(SemanticAnalyser* analyser) {
	auto ls_fun = default_version->function;
	ls_fun->args.clear();
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto& clazz = type.getArgumentType(i).raw_type->getClass();
		LSClass* arg_class = (LSClass*) analyser->vm->system_vars[clazz];
		if (arg_class != nullptr) {
			ls_fun->args.push_back((LSValue*) arg_class);
		} else {
			ls_fun->args.push_back(analyser->vm->system_vars["Value"]);
		}
	}
	auto return_class_name = type.getReturnType().raw_type->getClass();
	LSClass* return_class = (LSClass*) analyser->vm->system_vars[return_class_name];
	if (return_class != nullptr) {
		ls_fun->return_type = (LSValue*) return_class;
	} else {
		ls_fun->return_type = analyser->vm->system_vars["Value"];
	}
}

Type Function::version_type(std::vector<Type> version) const {
	if (versions.find(version) != versions.end()) {
		return versions.at(version)->type;
	}
	return type;
}

void Function::must_return(SemanticAnalyser*, const Type& type) {
	if (type == Type::POINTER) {
		generate_default_version = true;
	}
}

Compiler::value Function::compile(Compiler& c) const {

	// std::cout << "Function::compile() " << version << " " << has_version << std::endl;

	((Function*) this)->compiled = true;

	// Compile default version
	if (is_main_function || generate_default_version) {
		compile_version_internal(c, type.getArgumentTypes(), default_version);
	}

	// Add captures (for sub functions only)
	for (auto& version : ((Function*) this)->versions) {
		compile_version_internal(c, version.first, version.second);
	}

	if (has_version) {
		return compile_version(c, version);
	} else {
		// Return default version
		return c.new_pointer(default_version->function);
	}
}

Compiler::value Function::compile_version(Compiler& c, std::vector<Type> args) const {
	// std::cout << "Function::compile_version(" << args << ")" << std::endl;
	if (!compiled) {
		compile(c);
	}
	if (versions.find(args) == versions.end()) {
		std::cout << "/!\\ Version " << args << " not found!" << std::endl;
		return c.new_pointer(nullptr);
	}
	return c.new_pointer(versions.at(args)->function);
}

void Function::compile_version_internal(Compiler& c, std::vector<Type> args, Version* version) const {
	// std::cout << "Function::compile_version_internal(" << args << ")" << std::endl;

	auto ls_fun = version->function;

	Compiler::value jit_fun;
	if (!is_main_function) {
		jit_fun = c.new_pointer(ls_fun);
		for (const auto& cap : captures) {
			jit_value_t jit_cap;
			if (cap->scope == VarScope::LOCAL) {
				auto f = dynamic_cast<Function*>(cap->value);
				if (cap->has_version && f) {
					jit_cap = f->compile_version(c, cap->version).v;
				} else {
					jit_cap = c.get_var(cap->name).v;
				}
			} else if (cap->scope == VarScope::CAPTURE) {
				jit_cap = c.insn_get_capture(cap->parent_index, cap->initial_type).v;
			} else {
				jit_cap = jit_value_get_param(c.F, 1 + cap->index);
			}
			if (cap->initial_type.nature != Nature::POINTER) {
				jit_cap = VM::value_to_pointer(c.F, jit_cap, cap->initial_type);
			}
			c.function_add_capture(jit_fun, {jit_cap, cap->type});
		}
	}

	unsigned arg_count = arguments.size() + 1;
	vector<jit_type_t> params = {LS_POINTER}; // first arg is the function pointer
	for (unsigned i = 0; i < arg_count - 1; ++i) {
		Type t = version->type.getArgumentType(i);
		params.push_back(VM::get_jit_type(t));
	}
	jit_type_t return_type = VM::get_jit_type(version->type.getReturnType());
	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arg_count, 1);
	auto jit_function = jit_function_create(c.vm->jit_context, signature);
	jit_function_set_meta(jit_function, 12, new std::string(name), nullptr, 0);
	jit_function_set_meta(jit_function, 13, new std::string(file), nullptr, 0);
	jit_type_free(signature);
	jit_insn_uses_catcher(jit_function);

	c.enter_function(jit_function);

	// System internal variables (for main function only)
	if (is_main_function) {
		for (auto var : c.vm->system_vars) {
			auto name = var.first;
			auto value = var.second;
			auto val = c.new_pointer(value);
			c.vm->internals.insert(pair<string, jit_value_t>(name, val.v));
		}
	}

	auto res = version->body->compile(c);

	jit_insn_return(jit_function, res.v);

	// catch (ex)
	auto ex = jit_insn_start_catcher(jit_function);
	auto catchers = c.catchers.back();
	if (catchers.size() > 0) {
		for (size_t i = 0; i < catchers.size() - 1; ++i) {
			auto ca = catchers[i];
			jit_insn_branch_if_pc_not_in_range(jit_function, ca.start, ca.end, &ca.next);
			jit_insn_branch(jit_function, &ca.handler);
			jit_insn_label(jit_function, &ca.next);
		}
		jit_insn_branch(jit_function, &catchers.back().handler);
	} else {
		c.delete_function_variables();
		if (is_main_function) {
			c.vm->store_exception(jit_function, ex);
		} else {
			jit_insn_rethrow_unhandled(jit_function);
		}
	}

	jit_function_compile(jit_function);

	if (!is_main_function) {
		c.leave_function();
		// Create a function : 1 op
		c.inc_ops(1);
	}

	// Function are always pointers for now
	// functions as a simple pointer value can be like :
	// {c.new_pointer(f).v, type};
	void* f = jit_function_to_closure(jit_function);
	ls_fun->function = f;
	version->function = ls_fun;
}

Value* Function::clone() const {
	auto f = new Function();
	f->lambda = lambda;
	f->name = name;
	f->body = (Block*) body->clone();
	for (const auto& a : arguments) {
		f->arguments.push_back(a);
	}
	f->references = references;
	for (const auto& d : defaultValues) {
		if (d != nullptr) {
			f->defaultValues.push_back(d->clone());
		} else {
			f->defaultValues.push_back(nullptr);
		}
	}
	return f;
}

}
