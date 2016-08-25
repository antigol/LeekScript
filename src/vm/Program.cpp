#include "Program.hpp"
#include "Context.hpp"
#include "value/LSNull.hpp"
#include "value/LSArray.hpp"
#include <chrono>
#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticException.hpp"

using namespace std;

namespace ls {

extern map<string, jit_value_t> internals;

Program::Program(const std::string& code) {
	this->code = code;
	main = nullptr;
	closure = nullptr;
}

Program::~Program() {
	if (main != nullptr) {
		delete main;
	}
	for (auto v : system_vars) {
		delete v.second;
	}
}

double Program::compile(VM& vm, const std::string& ctx, const ExecMode mode) {

	auto compile_start = chrono::high_resolution_clock::now();

	LexicalAnalyser lex;
	vector<Token> tokens = lex.analyse(code);

	if (lex.errors.size()) {
		if (mode == ExecMode::TEST) {
			throw lex.errors[0];
		}
		for (auto error : lex.errors) {
			cout << "Line " << error.line << " : " <<  error.message() << endl;
		}
		return -1;
	}

	SyntaxicAnalyser syn;
	this->main = syn.analyse(tokens);

	if (syn.getErrors().size() > 0) {
		if (mode == ExecMode::COMMAND_JSON) {

			cout << "{\"success\":false,\"errors\":[";
			for (auto error : syn.getErrors()) {
				cout << "{\"line\":" << error->token->line << ",\"message\":\"" << error->message << "\"}";
			}
			cout << "]}" << endl;
			return -1;

		} else {
			for (auto error : syn.getErrors()) {
				cout << "Line " << error->token->line << " : " <<  error->message << endl;
			}
			return -1;
		}
	}

	Context context { ctx };

	SemanticAnalyser sem;
	sem.analyse(this, &context, vm.modules);

	/*
	 * Debug
	 */
	#if DEBUG > 0
		cout << "Program: "; print(cout, true);
	#endif

	if (sem.errors.size()) {

		if (mode == ExecMode::COMMAND_JSON) {
			cout << "{\"success\":false,\"errors\":[]}" << endl;
		} else if (mode == ExecMode::TEST) {
			delete this;
			throw sem.errors[0];
		} else {
			for (auto e : sem.errors) {
				cout << "Line " << e.line << " : " << e.message() << endl;
			}
		}
		return -1;
	}

	// Compilation
	internals.clear();

	this->compile_main(context);

	auto compile_end = chrono::high_resolution_clock::now();

	long compile_time_ns = chrono::duration_cast<chrono::nanoseconds>(compile_end - compile_start).count();
	double compile_time_ms = (((double) compile_time_ns / 1000) / 1000);

	return compile_time_ms;
}

void Program::compile_main(Context& context) {

	Compiler c;

	jit_init();
	jit_context_t jit_context = jit_context_create();
	jit_context_build_start(jit_context);

	jit_type_t params[0] = {};
	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(main->type.getReturnType()), params, 0, 0);
	jit_function_t F = jit_function_create(jit_context, signature);
	jit_insn_uses_catcher(F);
	c.enter_function(F);

	compile_jit(c, context, false);

	// catch (ex) {
	jit_value_t ex = jit_insn_start_catcher(F);
	VM::print_int(F, ex);
	jit_insn_return(F, LS_CREATE_POINTER(F, LSNull::get()));

	jit_function_compile(F);
	jit_context_build_end(jit_context);

	closure = jit_function_to_closure(F);
}

LSValue* Program::execute() {

	Type output_type = main->type.getReturnType();

	if (output_type == Type::BOOLEAN) {
		auto fun = (bool (*)()) closure;
		return LSBoolean::get(fun());
	}
	if (output_type == Type::INTEGER) {
		auto fun = (int (*)()) closure;
		return LSNumber::get((double) fun());
	}
	if (output_type == Type::FLOAT) {
		auto fun = (double (*)()) closure;
		return LSNumber::get(fun());
	}
	if (output_type == Type::LONG) {
		auto fun = (long (*)()) closure;
		return LSNumber::get(fun());
	}
	if (output_type.raw_type == RawType::FUNCTION and output_type.nature == Nature::VALUE) {
		auto fun = (void* (*)()) closure;
		return new LSFunction(fun());
	}
	auto fun = (LSValue* (*)()) closure;
	return fun();
}

void Program::print(ostream& os, bool debug) const {
	main->body->print(os, 0, debug);
	cout << endl;
}

std::ostream& operator << (std::ostream& os, const Program* program) {
	program->print(os, false);
	return os;
}

extern map<string, jit_value_t> internals;

LSArray<LSValue*>* Program_create_array() {
	return new LSArray<LSValue*>();
}
void Program_push_null(LSArray<LSValue*>* array, int) {
	array->push_clone(LSNull::get());
}
void Program_push_boolean(LSArray<LSValue*>* array, int value) {
	array->push_clone(LSBoolean::get(value));
}
void Program_push_integer(LSArray<LSValue*>* array, int value) {
	array->push_clone(LSNumber::get(value));
}
void Program_push_function(LSArray<LSValue*>* array, void* value) {
	array->push_clone(new LSFunction(value));
}
void Program_push_pointer(LSArray<LSValue*>* array, LSValue* value) {
	array->push_clone(value);
}

void Program::compile_jit(Compiler& c, Context& context, bool toplevel) {

	// System internal variables
	for (auto var : system_vars) {

		string name = var.first;
		LSValue* value = var.second;

		jit_value_t jit_val = LS_CREATE_POINTER(c.F, value);
		internals.insert(pair<string, jit_value_t>(name, jit_val));
	}

	// User context variables
	if (toplevel) {
		for (auto var : context.vars) {

			string name = var.first;
			LSValue* value = var.second;

			jit_value_t jit_var = jit_value_create(c.F, LS_POINTER);
			jit_value_t jit_val = LS_CREATE_POINTER(c.F, value);
			jit_insn_store(c.F, jit_var, jit_val);

			c.add_var(name, jit_var, Type(value->getRawType(), Nature::POINTER), false);

			value->refs++;
		}
	}

	jit_value_t res = main->body->compile(c);
	jit_insn_return(c.F, res);

	/*
	if (toplevel) {

		// Push program res
		jit_type_t array_sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
		jit_value_t array = jit_insn_call_native(F, "new", (void*) &Program_create_array, array_sig, {}, 0, JIT_CALL_NOTHROW);

		jit_type_t push_args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t push_sig_pointer = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);

		jit_value_t push_args[2] = {array, res};
		jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, push_args, 2, 0);

		VM::delete_obj(F, res);

//		cout << "GLOBALS : " << globals.size() << endl;

		for (auto g : c.get_vars()) {

			string name = g.first;
			Type type = globals_types[name];

			if (globals_ref[name] == true) {
//				cout << name << " is ref, continue" << endl;
				continue;
			}

//			cout << "save in context : " << name << ", type: " << type << endl;
//			cout << "jit_val: " << g.second << endl;

			jit_value_t var_args[2] = {array, g.second};

			if (type.nature == Nature::POINTER) {

//				cout << "save pointer" << endl;
				jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, var_args, 2, 0);

//				cout << "delete global " << g.first << endl;
				if (type.must_manage_memory()) {
					VM::delete_obj(F, g.second);
				}

			} else {
//				cout << "save value" << endl;
				if (type.raw_type == RawType::NULLL) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_null, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::BOOLEAN) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_boolean, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::INTEGER) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_integer, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::FLOAT) {
					jit_type_t args_float[2] = {JIT_POINTER, JIT_FLOAT};
					jit_type_t sig_push_float = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args_float, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_float, sig_push_float, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::FUNCTION) {
					jit_insn_call_native(F, "push", (void*) &Program_push_function, push_sig_pointer, var_args, 2, JIT_CALL_NOTHROW);
				}
			}
		}
		jit_insn_return(F, array);
	}
	*/
}

}
