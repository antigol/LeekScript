#include "FunctionCall.hpp"
#include "ObjectAccess.hpp"

using namespace std;

namespace ls {

FunctionCall::FunctionCall() {
	function = nullptr;
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

// DONE 1
void FunctionCall::analyse_help(SemanticAnalyser* analyser)
{
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		req_method_type = Type::FUNCTION;

		oa->object->analyse(analyser);

		req_method_type.add_argument_type(oa->object->type);

		for (size_t i = 0; i < arguments.size(); ++i) {
			Value* a = arguments[i];
			a->analyse(analyser);
			req_method_type.add_argument_type(a->type);
		}

		req_method_type.set_return_type(Type::UNKNOWN);

		type = Type::UNKNOWN;

	} else {

		function->analyse(analyser);
		for (Value* a : arguments) {
			a->analyse(analyser);
		}

		// Convertion
		type = function->type.return_type().image_conversion();
	}
}

void FunctionCall::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		Type generic;

		while (true) {
			// get methods
			methods = analyser->get_method(oa->field->content, req_method_type);
			if (methods.empty()) {
				add_error(analyser, SemanticException::METHOD_NOT_FOUND);
				return;
			}

			// compute generic
			generic = methods[0].type;
			for (size_t i = 1; i < methods.size(); ++i) {
				generic = Type::union_of(generic, methods[i].type);
			}

			if (!Type::intersection(type, generic.return_type().image_conversion(), &type)) {
				add_error(analyser, SemanticException::TYPE_MISMATCH);
			}

			// reanalyse -> req_type
			Type new_req_method_type = Type::FUNCTION;
			new_req_method_type.set_return_type(type.fiber_conversion());
			oa->object->reanalyse(analyser, generic.argument_type(0));
			new_req_method_type.add_argument_type(oa->object->type);
			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->reanalyse(analyser, generic.argument_type(i + 1));
				new_req_method_type.add_argument_type(a->type);
			}

			// if better redo
			if (new_req_method_type != req_method_type) {
				req_method_type = new_req_method_type;
			} else {
				break;
			}
		}

		oa->type = generic; // only for debug

	} else {
		Type req_fun_type = Type::FUNCTION;
		for (size_t i = 0; i < arguments.size(); ++i) {
			Value* a = arguments[i];
			a->reanalyse(analyser, req_fun_type.argument_type(i));
			req_fun_type.set_argument_type(i, a->type);
		}
		req_fun_type.set_return_type(type.fiber_conversion());

		Type old_fun_type = function->type;
		function->reanalyse(analyser, req_fun_type);

		while (old_fun_type != function->type) {
			// The function.type has changed !
			req_fun_type = function->type;
			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->reanalyse(analyser, req_fun_type.argument_type(i));
				req_fun_type.set_argument_type(i, a->type);
			}

			old_fun_type = function->type;
			function->reanalyse(analyser, req_fun_type);
		}

		// Convertion
		if (!Type::intersection(type, function->type.return_type().image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}
}

void FunctionCall::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		oa->object->finalize(analyser, Type::UNKNOWN);
		Type req_type = Type::FUNCTION;
		req_type.set_return_type(type.fiber_conversion());
		req_type.add_argument_type(oa->object->type);
		for (size_t i = 0; i < arguments.size(); ++i) {
			Value* a = arguments[i];
			req_type.add_argument_type(a->type);
		}
		methods = analyser->get_method(oa->field->content, req_type);
		if (methods.empty()) {
			add_error(analyser, SemanticException::METHOD_NOT_FOUND);
			return;
		}
		Type method_type = methods[0].type;

		method_type.set_argument_type(0, oa->object->type);
		if (!Type::intersection(method_type, methods[0].type, &method_type)) {
			add_error(analyser, SemanticException::METHOD_NOT_FOUND);
		}

		for (size_t i = 0; i < arguments.size(); ++i) {
			Value* a = arguments[i];
			a->finalize(analyser, method_type.argument_type(i + 1));
			method_type.set_argument_type(i + 1, a->type);
		}
		if (!Type::intersection(method_type, methods[0].type, &method_type)) {
			add_error(analyser, SemanticException::METHOD_NOT_FOUND);
		}

		if (!Type::intersection(type, method_type.return_type().image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		oa->type = method_type; // only for debug
		type.make_it_pure();

		methods[0].type = method_type;
		assert(method_type.is_pure() || !analyser->errors.empty());

	} else {

		Type req_fun_type = function->type;
		req_fun_type.set_return_type(type.fiber_conversion());

		for (size_t i = 0; i < arguments.size(); ++i) {
			Value* a = arguments[i];
			a->finalize(analyser, function->type.argument_type(i));
			req_fun_type.set_argument_type(i, a->type);
		}
		function->finalize(analyser, req_fun_type);

		if (!Type::intersection(type, function->type.return_type().image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();

	}
}

jit_value_t FunctionCall::compile(Compiler& c) const
{
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		vector<jit_value_t> args;
		vector<jit_type_t> args_types;
		args.push_back(oa->object->compile(c));
		args_types.push_back(oa->object->type.jit_type());
		for (size_t i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
			args_types.push_back(arguments[i]->type.jit_type());
		}
		Type rt = methods[0].type.return_type();
		return Compiler::compile_convert(c.F, Compiler::call_native(c.F, rt.jit_type(), args_types, methods[0].addr, args), rt, type);

	} else {

		jit_label_t label_end = jit_label_undefined;

		jit_value_t fun = function->compile(c);
		jit_value_t res = nullptr;

		if (function->type.return_type() == Type::VOID) {
			jit_insn_branch_if_not(c.F, fun, &label_end);
		} else {
			res = jit_value_create(c.F, function->type.return_type().jit_type());

			jit_label_t label_ok= jit_label_undefined;
			jit_insn_branch_if(c.F, fun, &label_ok);
			jit_insn_store(c.F, res, VM::create_default(c.F, function->type.return_type()));
			jit_insn_branch(c.F, &label_end);
			jit_insn_label(c.F, &label_ok);
		}

		vector<jit_value_t> args;
		vector<jit_type_t> args_types;

		for (size_t i = 0; i < arguments.size(); ++i) {

			args.push_back(arguments[i]->compile(c));
			args_types.push_back(function->type.argument_type(i).jit_type());

			// TODO this is the job of the function
			if (function->type.argument_type(i).must_manage_memory()) {
				args[i] = VM::move_inc_obj(c.F, args[i]);
			}
		}

		jit_type_t jit_return_type = function->type.return_type().jit_type();

		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_return_type, args_types.data(), arguments.size(), 0);
		jit_value_t val = jit_insn_call_indirect(c.F, fun, sig, args.data(), arguments.size(), 0);

		if (function->type.return_type() != Type::VOID) {
			jit_insn_store(c.F, res, val);
		}

		// TODO this is the job of the function
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

}
