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
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"

using namespace std;

namespace ls {

FunctionCall::FunctionCall(std::shared_ptr<Token> t) : token(t) {
	function = nullptr;
	type = Type::UNKNOWN;
	std_func = nullptr;
	this_ptr = nullptr;
	constant = false;
}

FunctionCall::~FunctionCall() {
	delete function;
	for (const auto& arg : arguments) {
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
		os << " " << types;
	}
}

Location FunctionCall::location() const {
	return {function->location().start, closing_parenthesis->location.end};
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// std::cout << "FunctionCall::analyse(" << req_type << ")" << std::endl;

	// Analyse the function (can be anything here)
	function->analyse(analyser, Type::UNKNOWN);

	// Find the function object
	function_object = dynamic_cast<Function*>(function);
	if (!function_object) {
		auto vv = dynamic_cast<VariableValue*>(function);
		if (vv) {
			function_object = static_cast<Function*>(vv->var->value);
		}
	}

	// The function call be called?
	if (function->type.raw_type != RawType::UNKNOWN and
		function->type.raw_type != RawType::FUNCTION and
		function->type.raw_type != RawType::CLASS) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE, location(), function->location(), {function->to_string()}});
	}

	// Analyse all arguments a first time
	for (auto& arg : arguments) {
		arg->analyse(analyser, Type::UNKNOWN);
	}

	// Standard library constructors TODO better
	auto vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") type = Type::INTEGER;
		if (vv->name == "Boolean") type = Type::BOOLEAN;
		if (vv->name == "String") type = Type::STRING;
		if (vv->name == "Array") type = Type::PTR_ARRAY;
		if (vv->name == "Object") type = Type::OBJECT;
	}

	// Detect standard library functions
	auto oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		auto field_name = oa->field->content;
		auto object_type = oa->object->type;

		vector<Type> arg_types;
		for (auto arg : arguments) {
			arg_types.push_back(arg->type);
		}
		std::string args_string = "";
		for (unsigned i = 0; i < arg_types.size(); ++i) {
			std::ostringstream oss;
			oss << arg_types[i];
			if (i > 0) args_string += ", ";
			args_string += oss.str();
		}

		if (object_type.raw_type == RawType::CLASS) { // String.size("salut")

			string clazz = ((VariableValue*) oa->object)->name;
			LSClass* object_class = (LSClass*) analyser->vm->system_vars[clazz];
			StaticMethod* sm = object_class->getStaticMethod(oa->field->content, arg_types);

			if (sm != nullptr) {
				std_func = sm->addr;
				function->type = sm->type;
				is_native_method = sm->native;
			} else {
				LSClass* value_class = (LSClass*) analyser->vm->system_vars["Value"];
				Method* m = value_class->getMethod(oa->field->content, object_type, arg_types);

				if (m != nullptr) {
					this_ptr = oa->object;
					this_ptr->analyse(analyser, m->obj_type);
					std_func = m->addr;
					function->type = m->type;
					is_native_method = m->native;
				} else {
					analyser->add_error({SemanticError::Type::STATIC_METHOD_NOT_FOUND, location(), oa->field->location, {clazz + "::" + oa->field->content + "(" + args_string + ")"}});
				}
			}
		} else {  // "salut".size()

			Method* m = nullptr;
			if (object_type.raw_type != RawType::UNKNOWN) {
				LSClass* object_class = (LSClass*) analyser->vm->system_vars[object_type.clazz];
				m = object_class->getMethod(oa->field->content, object_type, arg_types);
				// std::cout << "Method " << oa->field->content << " found : " << arg_types << std::endl;
			}
			if (m == nullptr) {
				LSClass* value_class = (LSClass*) analyser->vm->system_vars["Value"];
				m = value_class->getMethod(oa->field->content, object_type, arg_types);
			}
			if (m != nullptr) {
				this_ptr = oa->object;
				this_ptr->analyse(analyser, m->obj_type);
				std_func = m->addr;
				function->type = m->type;
				is_native_method = m->native;
			} else {
				if (object_type.raw_type != RawType::UNKNOWN) {
					std::ostringstream obj_type_ss;
					obj_type_ss << object_type;
					analyser->add_error({SemanticError::Type::METHOD_NOT_FOUND, location(), oa->field->location, {obj_type_ss.str() + "." + oa->field->content + "(" + args_string + ")"}});
				} else {
					is_unknown_method = true;
					object = oa->object;
				}
			}
		}
	}

	// Operator function? TODO better, no special behavior
	vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		auto name = vv->name;
		if (name == "+" or name == "-" or name == "*" or name == "/" or name == "**" or name == "%") {
			bool isByValue = true;
			Type effectiveType;
			for (auto& arg : arguments) {
				arg->analyse(analyser, Type::UNKNOWN);
				effectiveType = arg->type;
				if (arg->type.nature != Nature::VALUE) {
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

	// Check arguments count
	std::vector<Type> arg_types;
	bool arguments_valid = arguments.size() <= function->type.getArgumentTypes().size();
	auto total_arguments_passed = std::max(arguments.size(), function->type.getArgumentTypes().size());
	size_t a = 0;
	for (auto& argument_type : function->type.getArgumentTypes()) {
		if (a < arguments.size()) {
			// OK, the argument is present in the call
			arguments.at(a)->analyse(analyser, argument_type);
			if (function->type.getArgumentType(a).raw_type == RawType::FUNCTION) {
				arguments.at(a)->will_take(analyser, function->type.getArgumentType(a).arguments_types, 1);
				arguments.at(a)->set_version(function->type.getArgumentType(a).arguments_types);
			}
			arg_types.push_back(arguments.at(a)->type);
		} else if (function->type.argumentHasDefault(a)) {
			// OK, there's no argument in the call but a default value is set.
			if (function_object) {
				arg_types.push_back(function_object->defaultValues.at(a)->type);
			}
		} else {
			// Missing argument
			arguments_valid = false;
			total_arguments_passed--;
		}
		a++;
	}

	if (function->type.raw_type == RawType::FUNCTION and !arguments_valid) {
		analyser->add_error({SemanticError::Type::WRONG_ARGUMENT_COUNT,	location(), location(), {
			function->to_string(),
			std::to_string(function->type.getArgumentTypes().size()),
			std::to_string(total_arguments_passed)
		}});
		type = Type::UNKNOWN;
		return;
	}

	function->will_take(analyser, arg_types, 1);
	function->set_version(arg_types);

	// Get the function type
	function_type = function->type;
	if (function_object) {
		if (function_object->versions.size() && function_object->versions.find(arg_types) != function_object->versions.end()) {
			function_type = function_object->versions.at(arg_types)->type;
		}
	}

	// The function is a variable
	if (vv and vv->var and vv->var->value) {
		// Recursive function
		if (vv->var->name == analyser->current_function()->name) {
			type = analyser->current_function()->getReturnType();
		} else {
			auto ret_type = function_type.getReturnType();
			if (ret_type.raw_type != RawType::UNKNOWN) {
				type = ret_type;
			}
		}
	} else {
		auto ret_type = function_type.getReturnType();
		if (ret_type.nature != Nature::UNKNOWN) {
			type = ret_type;
		}
	}

	a = 0;
	for (auto& arg : arguments) {
		auto t = function_type.getArgumentType(a);
		if (t == Type::UNKNOWN) {
			t = Type::POINTER;
		}
		arg->analyse(analyser, t);
		a++;
	}

	return_type = function_type.getReturnType();

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
	types = type;
}

bool FunctionCall::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	if (auto vv = dynamic_cast<VariableValue*>(function)) {
		vv->will_take(analyser, args, level + 1);
	}
	return false;
}

Compiler::value FunctionCall::compile(Compiler& c) const {

	// std::cout << "FunctionCall::compile(" << function_type << ")" << std::endl;

	/** Standard library constructors : Array(), Number() */
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Boolean") {
			jit_value_t n = jit_value_create_nint_constant(c.F, LS_INTEGER, 0);
			if (type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, n, Type::BOOLEAN), type};
			}
			return {n, type};
		}
		if (vv->name == "Number") {
			jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
			if (type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, n, Type::INTEGER), type};
			}
			return {n, type};
		}
		if (vv->name == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile(c);
			}
			return {c.new_pointer(new LSString("")).v, type};
		}
		if (vv->name == "Array") {
			return {c.new_pointer(new LSArray<LSValue*>()).v, type};
		}
		if (vv->name == "Object") {
			return {c.new_pointer(new LSObject()).v, type};
		}
	}

	/** Standard function call on object : "hello".size() */
	if (this_ptr != nullptr) {

		vector<Compiler::value> args = { this_ptr->compile(c) };
		this_ptr->compile_end(c);
		vector<LSValueType> lsvalue_types = { (LSValueType) this_ptr->type.id() };
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			lsvalue_types.push_back(function->type.getArgumentType(i).id());
		}
		c.insn_check_args(args, lsvalue_types);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, type), type};
		}
		return res;
	}

	/** Static standard function call : Number.char(65) */
	if (std_func != nullptr) {

		vector<Compiler::value> args;
		vector<LSValueType> lsvalue_types;
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			lsvalue_types.push_back((LSValueType) function->type.getArgumentType(i).id());
		}
		c.insn_check_args(args, lsvalue_types);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, type), type};
		}
		return res;
	}

	/** Operator functions : +(1, 2) */
	VariableValue* f = dynamic_cast<VariableValue*>(function);

	if (f != nullptr) {
		if (function->type.getArgumentType(0).nature == Nature::VALUE
			and function->type.getArgumentType(1).nature == Nature::VALUE) {

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
				auto v0 = arguments[0]->compile(c);
				auto v1 = arguments[1]->compile(c);
				jit_value_t ret = jit_func(c.F, v0.v, v1.v);

				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, ret, type), type};
				}
				return {ret, type};
			}
		}
	}

	/** Default function : f(12) */
	jit_value_t fun;
	auto ls_fun_addr = c.new_pointer(nullptr);
	auto jit_object = c.new_pointer(nullptr);

	if (is_unknown_method) {
		auto oa = static_cast<ObjectAccess*>(function);
		jit_object = c.insn_load(((LeftValue*) object)->compile_l(c));
		// jit_object = object->compile(c);
		auto k = c.new_pointer(&oa->field->content);
		ls_fun_addr = c.insn_call(type, {jit_object, k}, (void*) +[](LSValue* object, std::string* key) {
			return object->attr(*key);
		});
		fun = jit_insn_load_relative(c.F, ls_fun_addr.v, 24, LS_POINTER);
	} else {
		ls_fun_addr = function->compile(c);
		fun = jit_insn_load_relative(c.F, ls_fun_addr.v, 24, LS_POINTER);
	}

	/** Arguments */
	size_t offset = 1;

	size_t arg_count = function_type.getArgumentTypes().size() + offset;
	vector<Compiler::value> args;
	vector<jit_type_t> args_types;
	vector<LSValueType> lsvalue_types;

	if (is_unknown_method) {
		// add 'this' object as first argument
		args.push_back(jit_object);
		args_types.push_back(VM::get_jit_type(object->type));
		lsvalue_types.push_back(object->type.id());
	} else {
		// Function pointer as first argument
		args.push_back(ls_fun_addr);
		args_types.push_back(LS_POINTER);
		lsvalue_types.push_back(Type::FUNCTION_P.id());
	}

	for (size_t i = 0; i < arg_count - offset; ++i) {
		if (i < arguments.size()) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			if (function_type.getArgumentType(i) == Type::MPZ &&
				arguments.at(i)->type != Type::MPZ_TMP) {
				args.at(offset + i) = c.insn_clone_mpz(args.at(offset + i));
			}
		} else {
			args.push_back(function_object->defaultValues.at(i)->compile(c));
		}
		args_types.push_back(VM::get_jit_type(function_type.getArgumentType(i)));
		lsvalue_types.push_back(function_type.getArgumentType(i).id());
		if (function_type.getArgumentType(i).must_manage_memory()) {
			args.at(offset + i) = c.insn_move_inc(args.at(offset + i));
		}
	}

	jit_insn_mark_offset(c.F, location().start.line);

	// TODO : some tests are failing with this
	// c.insn_check_args(args, lsvalue_types);

	jit_type_t jit_return_type = VM::get_jit_type(type);

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_return_type, args_types.data(), args_types.size(), 1);
	vector<jit_value_t> jit_args;
	for (const auto& a : args) jit_args.push_back(a.v);
	jit_value_t ret = jit_insn_call_indirect(c.F, fun, sig, jit_args.data(), arg_count, 0);
	// FIXME Not a fail : need two free() here ^^
	jit_type_free(sig);
	jit_type_free(sig);

	// Destroy temporary arguments
	for (size_t i = 0; i < arg_count - offset; ++i) {
		if (function_type.getArgumentType(i).must_manage_memory()) {
			c.insn_delete(args.at(offset + i));
		}
		if (function_type.getArgumentType(i) == Type::MPZ ||
			function_type.getArgumentType(i) == Type::MPZ_TMP) {
			c.insn_delete_mpz(args.at(offset + i));
		}
	}
	c.insn_delete_temporary(ls_fun_addr);

	if (is_unknown_method) {
		object->compile_end(c);
	} else {
		function->compile_end(c);
	}

	// Custom function call : 1 op
	c.inc_ops(1);

	if (return_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return {VM::value_to_pointer(c.F, ret, type), type};
	}
	return {ret, type};
}

Value* FunctionCall::clone() const {
	auto fc = new FunctionCall(token);
	fc->function = function->clone();
	for (const auto& a : arguments) {
		fc->arguments.push_back(a->clone());
	}
	fc->closing_parenthesis = closing_parenthesis;
	return fc;
}

}
