#include "FunctionCall.hpp"

#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-value.h>
#include <sstream>
#include <string>

#include "../../vm/Module.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Type.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSVar.hpp"
#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"

using namespace std;

namespace ls {

FunctionCall::FunctionCall() : method(Type::VOID, nullptr) {
	function = nullptr;
//	std_func = nullptr;
//	this_ptr = nullptr;
}

FunctionCall::~FunctionCall() {
	delete function;
	for (auto arg : arguments) {
		delete arg;
	}
}

void FunctionCall::print(std::ostream& os, int indent, bool debug) const {

	function->print(os, indent, debug);
	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		arguments.at(i)->print(os, indent, debug);
		if (i < arguments.size() - 1) {
			os << ", ";
		}
	}
	os << ")";
	if (debug) {
		os << " " << type;
	}
}

unsigned FunctionCall::line() const {
	return 0;
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	constant = false;
	method = Method(Type::VOID, nullptr);


	/*
	// Standard library constructors
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") type = Type::INTEGER;
		if (vv->name == "Boolean") type = Type::BOOLEAN;
		if (vv->name == "String") type = Type::STRING;
		if (vv->name == "Array") type = Type::PTR_ARRAY;
		if (vv->name == "Object") type = Type::OBJECT;
	}
	*/
	// Detect standard library functions
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		oa->object->analyse(analyser, Type::UNKNOWN);
		Module* module = analyser->module_by_name(oa->object->type.raw_type->clazz());

		if (module == nullptr) {
			analyser->add_error({ SemanticException::METHOD_NOT_FOUND, line() });
			return;
		}

		vector<Type> args_types;
		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->preanalyse(analyser); // preanalyse [] will give [].type = vec<??>
			args_types.push_back(arguments[i]->type);
		}

		Method method = module->get_method_implementation(oa->field->content, oa->object->type, req_type, args_types);

		if (method.addr == nullptr) {
			stringstream oss;
			oa->print(oss, 0, false);
			analyser->add_error({ SemanticException::METHOD_NOT_FOUND, line(), oss.str() });
			return;
		}

		this->method = method;
		oa->type = method.type;

		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->analyse(analyser, method.type.argument_type(i + 1));
		}

		if (req_type != Type::UNKNOWN) {
			type = req_type;

			if (!method.type.return_type().can_be_convert_in(req_type)) {
				analyser->add_error({ SemanticException::TYPE_MISMATCH, line() });
			}
		} else {
			type = method.type.return_type();
		}

		return;




		/*
		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);
		if (vv != nullptr and vv->name == "Number") {

			if (field_name == "abs") {
				function->type.setArgumentType(0, Type::INTEGER);
				function->type.setReturnType(Type::INTEGER);
				is_static_native = true;
			} else if (field_name == "floor") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::INTEGER);
				is_static_native = true;
			} else if (field_name == "round") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "ceil") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::INTEGER);
				is_static_native = true;
			} else if (field_name == "cos") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "sin") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "max") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "min") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "sqrt") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			} else if (field_name == "pow") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
				is_static_native = true;
			}
			native_func = field_name;
		}*/

		/*
		if (!is_static_native) {

			Type object_type = oa->object->type;

			vector<Type> arg_types;
			for (auto arg : arguments) {
				arg_types.push_back(arg->type);
			}

			if (object_type.raw_type == &RawType::CLASS) { // String.size("salut")

				string clazz = ((VariableValue*) oa->object)->name;

				LSClass* object_class = (LSClass*) analyser->program->system_vars[clazz];

				StaticMethod* m = object_class->getStaticMethod(oa->field->content, arg_types);

				if (m != nullptr) {
					std_func = m->addr;
					function->type = m->type;
				} else {
					analyser->add_error({SemanticException::Type::STATIC_METHOD_NOT_FOUND, oa->field->line, oa->field->content});
				}

			} else { // "salut".size()

				if (object_type.raw_type == &RawType::INTEGER
					|| object_type.raw_type == &RawType::FLOAT) {

					if (field_name == "abs") {
						function->type.setArgumentType(0, Type::INTEGER);
						function->type.setReturnType(Type::INTEGER);
						is_native = true;
					} else if (field_name == "floor") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::INTEGER);
						is_native = true;
					} else if (field_name == "round") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "ceil") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::INTEGER);
						is_native = true;
					} else if (field_name == "cos") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "sin") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "max") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setArgumentType(1, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "min") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setArgumentType(1, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "sqrt") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					} else if (field_name == "pow") {
						function->type.setArgumentType(0, Type::FLOAT);
						function->type.setArgumentType(1, Type::FLOAT);
						function->type.setReturnType(Type::FLOAT);
						is_native = true;
					}
					native_func = field_name;
					oa->object->analyse(analyser, Type(object_type.raw_type, Nature::VALUE));
				}


				if (!is_native and object_type.raw_type != &RawType::UNKNOWN) {

					LSClass* object_class = (LSClass*) analyser->program->system_vars[object_type.clazz];

					Method* m = object_class->getMethod(oa->field->content, object_type, arg_types);

					if (m != nullptr) {

						this_ptr = oa->object;
						this_ptr->analyse(analyser, Type::POINTER);

						std_func = m->addr;
						function->type = m->type;

					} else {
						analyser->add_error({SemanticException::Type::METHOD_NOT_FOUND, oa->field->line, oa->field->content});
					}
				}
			}

			if (object_type == Type::PTR_ARRAY) {
				cout << "array" << endl;

				cout << "type before: " << function->type << endl;

				for (unsigned int i = 0; i < function->type.arguments_types.size(); ++i) {
					cout << "arg " << i << " type : " << function->type.getArgumentType(i) << endl;
					Type arg = function->type.getArgumentType(i);
					if (arg.raw_type == &RawType::FUNCTION) {
						for (unsigned int j = 0; j < arg.getArgumentTypes().size(); ++j) {
							if (arg.getArgumentType(j) == Type::PTR_ARRAY_ELEMENT) {
								cout << "set arg " << j << " : " << object_type.getElementType(0) << endl;
								arg.setArgumentType(j, object_type.getElementType(0));
								function->type.setArgumentType(i, arg);
							}
						}
					}
				}
				cout << "type after: " << function->type << endl;
			}

		}*/
	}
/*
	vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		string name = vv->name;
		if (name == "+" or name == "-" or name == "*" or name == "/" or name == "^" or name == "%") {
			bool isByValue = true;
			Type effectiveType;
			for (Value* arg : arguments) {
				arg->analyse(analyser, Type::UNKNOWN);
				effectiveType = arg->type;
				if (arg->type.raw_type->nature() != Nature::VALUE) {
					isByValue = false;
				}
			}
			if (isByValue) {
				function->type.setArgumentType(0, effectiveType);
				function->type.setArgumentType(1, effectiveType);
				function->type.setReturnType(effectiveType);
			}
			type = function->type.getReturnType();
		}
	}

	vector<Type> arg_types;
	for (auto arg : arguments) {
		arg_types.push_back(arg->type);
	}

	a = 0;
	for (Value* arg : arguments) {
		arg->analyse(analyser, function->type.getArgumentType(a));
		if (function->type.getArgumentType(a).raw_type == &RawType::FUNCTION) {
			arg->will_take(analyser, function->type.getArgumentType(a).arguments_types);
		}
		a++;
	}

	function->will_take(analyser, arg_types);

	// The function is a variable
	if (vv and vv->var and vv->var->value) {

		vv->var->will_take(analyser, arg_types);

		Type ret_type = vv->var->value->type.getReturnType();
		if (ret_type.raw_type != &RawType::UNKNOWN) {
			type = ret_type;
		}
	} else {
		Type ret_type = function->type.getReturnType();
		if (ret_type.raw_type->nature() != Nature::UNKNOWN) {
			type = ret_type;
		}
		/*
		if (ret_type.raw_type != &RawType::UNKNOWN) {
			type = ret_type;
		} else {
			// TODO : to be able to remove temporary variable we must know the nature
//			type = Type::POINTER; // When the function is unknown, the return type is a pointer
		}
		*//*
	}

	return_type = function->type.getReturnType();

	if (req_type.raw_type->nature() != Nature::UNKNOWN) {
		type.raw_type->nature() = req_type.raw_type->nature();
	}
	*/

	function->analyse(analyser, Type::UNKNOWN);

	if (function->type.raw_type != &RawType::FUNCTION) {
		std::ostringstream oss;
		function->print(oss);
		analyser->add_error({ SemanticException::CANNOT_CALL_VALUE, function->line(), oss.str() });
	}
	if (arguments.size() != function->type.arguments_types.size()) {
		std::ostringstream oss;
		function->print(oss);
		analyser->add_error({ SemanticException::NUMBER_ARGUMENTS_MISMATCH, function->line(), oss.str() });
	}

	for (size_t i = 0; i < arguments.size(); ++i) {
		arguments[i]->analyse(analyser, function->type.argument_type(i));
	}

	if (req_type != Type::UNKNOWN && function->type.return_type().can_be_convert_in(req_type)) {
		type = req_type;
	} else {
		type = function->type.return_type();
	}
}

/*
LSValue* create_float_object_3(double n) {
	return new LSVar(n);
}
LSValue* create_int_object_3(int n) {
	return new LSVar(n);
}*/

jit_value_t FunctionCall::compile(Compiler& c) const {

//	cout << "compile function call" << type << endl;

	/*
	 * Standard library constructors
	 *//*
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Boolean") {
			jit_value_t n = jit_value_create_nint_constant(c.F, LS_I32, 0);
			if (type.raw_type->nature() == Nature::LSVALUE) {
				return VM::value_to_pointer(c.F, n, Type::BOOLEAN);
			}
			return n;
		}
		if (vv->name == "Number") {
			jit_value_t n = LS_CREATE_I32(c.F, 0);
			if (type.raw_type->nature() == Nature::LSVALUE) {
				return VM::value_to_pointer(c.F, n, Type::INTEGER);
			}
			return n;
		}
		if (vv->name == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile(c);
			}
			return LS_CREATE_POINTER(c.F, new LSString(""));
		}
		if (vv->name == "Array") {
			return LS_CREATE_POINTER(c.F, new LSVec<LSValue*>());
		}
		if (vv->name == "Object") {
			return LS_CREATE_POINTER(c.F, new LSObject());
		}
	}

	/*
	 * Compile native static standard functions : Number.abs(12)
	 *//*
	if (is_static_native) {

		jit_value_t res = nullptr;

		if (native_func == "abs") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_abs(c.F, v);
		} else if (native_func == "floor") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_floor(c.F, v);
		} else if (native_func == "round") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_round(c.F, v);
		} else if (native_func == "ceil") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_ceil(c.F, v);
		} else if (native_func == "cos") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_cos(c.F, v);
		} else if (native_func == "sin") {
			jit_value_t v = arguments[0]->compile(c);
			res = jit_insn_sin(c.F, v);
		} else if (native_func == "max") {
			jit_value_t v1 = arguments[0]->compile(c);
			jit_value_t v2 = arguments[1]->compile(c);
			res = jit_insn_max(c.F, v1, v2);
		} else if (native_func == "min") {
			jit_value_t v1 = arguments[0]->compile(c);
			jit_value_t v2 = arguments[1]->compile(c);
			res = jit_insn_min(c.F, v1, v2);
		} else if (native_func == "sqrt") {
			jit_value_t v1 = arguments[0]->compile(c);
			res = jit_insn_sqrt(c.F, v1);
		} else if (native_func == "pow") {
			jit_value_t v1 = arguments[0]->compile(c);
			jit_value_t v2 = arguments[1]->compile(c);
			res = jit_insn_pow(c.F, v1, v2);
		}

		if (res != nullptr) {
			if (type.raw_type->nature() == Nature::LSVALUE) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;
		}
	}

	/*
	 * Native standard function call on object : 12.abs()
	 *//*
	if (is_native) {

		Value* object = ((ObjectAccess*) function)->object;

		jit_value_t res = nullptr;

		if (native_func == "abs") {
			jit_value_t v = object->compile(c);
			res = jit_insn_abs(c.F, v);
		} else if (native_func == "floor") {
			jit_value_t v = object->compile(c);
			res = jit_insn_floor(c.F, v);
		} else if (native_func == "round") {
			jit_value_t v = object->compile(c);
			res = jit_insn_round(c.F, v);
		} else if (native_func == "ceil") {
			jit_value_t v = object->compile(c);
			res = jit_insn_ceil(c.F, v);
		} else if (native_func == "cos") {
			jit_value_t v = object->compile(c);
			res = jit_insn_cos(c.F, v);
		} else if (native_func == "sin") {
			jit_value_t v = object->compile(c);
			res = jit_insn_sin(c.F, v);
		} else if (native_func == "max") {
			jit_value_t v1 = object->compile(c);
			jit_value_t v2 = arguments[0]->compile(c);
			res = jit_insn_max(c.F, v1, v2);
		} else if (native_func == "min") {
			jit_value_t v1 = object->compile(c);
			jit_value_t v2 = arguments[0]->compile(c);
			res = jit_insn_min(c.F, v1, v2);
		} else if (native_func == "sqrt") {
			jit_value_t v1 = object->compile(c);
			res = jit_insn_sqrt(c.F, v1);
		} else if (native_func == "pow") {
			jit_value_t v1 = object->compile(c);
			jit_value_t v2 = arguments[0]->compile(c);
			res = jit_insn_pow(c.F, v1, v2);
		}

		if (res != nullptr) {
			if (type.raw_type->nature() == Nature::LSVALUE) {
				return VM::value_to_pointer(c.F, res, type);
			}
			return res;
		}
	}

	/*
	 * Standard function call on object
	 *//*
	if (this_ptr != nullptr) {

//		cout << "compile std function " << function->type << endl;

		int arg_count = arguments.size() + 1;
		vector<jit_value_t> args = { this_ptr->compile(c) };
		vector<jit_type_t> args_types = { LS_POINTER };

		for (int i = 0; i < arg_count - 1; ++i) {
			args.push_back(arguments[i]->compile(c));

			args_types.push_back(VM::get_jit_type(function->type.getArgumentType(i)));
		}

		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(return_type), args_types.data(), arg_count, 0);

		jit_value_t res = jit_insn_call_native(c.F, "std_func", (void*) std_func, sig, args.data(), arg_count, JIT_CALL_NOTHROW);

		if (return_type.raw_type->nature() == Nature::VALUE and type.raw_type->nature() == Nature::LSVALUE) {
			return VM::value_to_pointer(c.F, res, type);
		}
		return res;
	}

	/*
	 * Static standard function call
	 *//*
	if (std_func != nullptr) {

//		cout << "compile static std function" << endl;

		int arg_count = arguments.size();
		vector<jit_value_t> args;
		vector<jit_type_t> args_types;

		for (int i = 0; i < arg_count; ++i) {
			args.push_back(arguments[i]->compile(c));
			args_types.push_back(VM::get_jit_type(function->type.getArgumentType(i)));
		}

		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(return_type), args_types.data(), arg_count, 0);

		jit_value_t res = jit_insn_call_native(c.F, "std_func", (void*) std_func, sig, args.data(), arg_count, JIT_CALL_NOTHROW);

		if (return_type.raw_type->nature() == Nature::VALUE and type.raw_type->nature() == Nature::LSVALUE) {
			return VM::value_to_pointer(c.F, res, type);
		}
		return res;
	}

	/*
	 * Operator functions
	 *//*
	VariableValue* f = dynamic_cast<VariableValue*>(function);

	if (f != nullptr) {
		if (function->type.getArgumentType(0).raw_type->nature() == Nature::VALUE
			and function->type.getArgumentType(1).raw_type->nature() == Nature::VALUE) {

			jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
			if (f->name == "+") {
				jit_func = &jit_insn_add;
			} else if (f->name == "-") {
				jit_func = &jit_insn_sub;
			} else if (f->name == "*" or f->name == "×") {
				jit_func = &jit_insn_mul;
			} else if (f->name == "/" or f->name == "÷") {
				jit_func = &jit_insn_div;
			} else if (f->name == "**") {
				jit_func = &jit_insn_pow;
			} else if (f->name == "%") {
				jit_func = &jit_insn_rem;
			}
			if (jit_func != nullptr) {
				jit_value_t v0 = arguments[0]->compile(c);
				jit_value_t v1 = arguments[1]->compile(c);
				jit_value_t ret = jit_func(c.F, v0, v1);

				if (type.raw_type->nature() == Nature::LSVALUE) {
					return VM::value_to_pointer(c.F, ret, type);
				}
				return ret;
			}
		}
	}
*/

	if (method.addr) {
		ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);

		vector<jit_value_t> args;
		vector<jit_type_t> args_types;
		args.push_back(oa->object->compile(c));
		args_types.push_back(oa->object->type.raw_type->jit_type());
		for (size_t i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
			args_types.push_back(arguments[i]->type.raw_type->jit_type());
		}
		Type rt = method.type.return_type();
		return Compiler::compile_convert(c.F, Compiler::call_native(c.F, rt.raw_type->jit_type(), args_types, method.addr, args), rt, type);
	}


	/*
	 * Default function
	 */
	jit_value_t fun = function->compile(c);
	jit_value_t res = function->type.return_type() != Type::VOID ? jit_value_create(c.F, VM::get_jit_type(function->type.return_type())) : nullptr;

	jit_label_t label_ok= jit_label_undefined;
	jit_label_t label_end = jit_label_undefined;
	if (function->type.return_type() == Type::VOID) {
		jit_insn_branch_if_not(c.F, fun, &label_end);
	} else {
		jit_insn_branch_if(c.F, fun, &label_ok);
		jit_insn_store(c.F, res, VM::create_default(c.F, function->type.return_type()));
		jit_insn_branch(c.F, &label_end);
		jit_insn_label(c.F, &label_ok);
	}

	vector<jit_value_t> args;
	vector<jit_type_t> args_types;

	for (size_t i = 0; i < arguments.size(); ++i) {

		args.push_back(arguments[i]->compile(c));
		args_types.push_back(VM::get_jit_type(function->type.argument_type(i)));

		if (function->type.argument_type(i).must_manage_memory()) {
			args[i] = VM::move_inc_obj(c.F, args[i]);
		}
	}

	jit_type_t jit_return_type = VM::get_jit_type(function->type.return_type());

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_return_type, args_types.data(), arguments.size(), 0);

	if (function->type.return_type() == Type::VOID) {
		jit_insn_call_indirect(c.F, fun, sig, args.data(), arguments.size(), 0);
	} else {
		jit_insn_store(c.F, res, jit_insn_call_indirect(c.F, fun, sig, args.data(), arguments.size(), 0));
	}

	// Destroy temporary arguments
	for (size_t i = 0; i < arguments.size(); ++i) {
		if (function->type.argument_type(i).must_manage_memory()) {
			VM::delete_ref(c.F, args[i]);
		}
	}

	jit_insn_label(c.F, &label_end);

	// Custom function call : 1 op
	VM::inc_ops(c.F, 1);

	return Compiler::compile_convert(c.F, res, function->type.return_type(), type);
}

}
