#include "FunctionCall.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"
#include "../jit/jit_general.hpp"

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

// DONE 2
void FunctionCall::analyse_help(SemanticAnalyser* analyser)
{
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);

	if (oa != nullptr) {
		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);

		if (vv && vv->name == "ls") {

			req_method_type = Type::FUNCTION;
			res_method_type = Type::FUNCTION;

			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->analyse(analyser);
				req_method_type.add_argument_type(a->type);
			}

			req_method_type.set_return_type(Type::UNKNOWN);

			type = Type::UNKNOWN;

		} else {

			req_method_type = Type::FUNCTION;
			res_method_type = Type::FUNCTION;

			oa->object->analyse(analyser);

			req_method_type.add_argument_type(oa->object->type);

			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->analyse(analyser);
				req_method_type.add_argument_type(a->type);
			}

			req_method_type.set_return_type(Type::UNKNOWN);

			type = Type::UNKNOWN;

		}

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

		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);

		if (vv && vv->name == "ls") {

			while (analyser->errors.empty()) {
				// get methods
				if (methods.size() != 1 || !res_method_type.is_pure()) {
					methods = analyser->get_method("ls", oa->field->content, req_method_type, &res_method_type);
					if (methods.empty()) {
						add_error(analyser, SemanticException::METHOD_NOT_FOUND);
						return;
					}
				}

				if (!Type::intersection(type, res_method_type.return_type().image_conversion(), &type)) {
					add_error(analyser, SemanticException::TYPE_MISMATCH);
				}

				// reanalyse -> req_type
				Type new_req_method_type = Type::FUNCTION;
				new_req_method_type.set_return_type(type.fiber_conversion());
				for (size_t i = 0; i < arguments.size(); ++i) {
					Value* a = arguments[i];
					a->reanalyse(analyser, res_method_type.argument_type(i));
					new_req_method_type.add_argument_type(a->type);
				}

				// if better redo
				if (new_req_method_type != req_method_type) {
					req_method_type = new_req_method_type;
				} else {
					break;
				}
			}

			oa->type = res_method_type; // only for debug

		} else {

			while (analyser->errors.empty()) {
				// get methods
				if (methods.size() != 1 || !res_method_type.is_pure()) {
					methods = analyser->get_method(req_method_type.argument_type(0).get_raw_type()->clazz(), oa->field->content, req_method_type, &res_method_type);
					if (methods.empty()) {
						add_error(analyser, SemanticException::METHOD_NOT_FOUND);
						return;
					}
				}

				if (!Type::intersection(type, res_method_type.return_type().image_conversion(), &type)) {
					add_error(analyser, SemanticException::TYPE_MISMATCH);
				}

				// reanalyse -> req_type
				Type new_req_method_type = Type::FUNCTION;
				new_req_method_type.set_return_type(type.fiber_conversion());
				oa->object->reanalyse(analyser, res_method_type.argument_type(0));
				new_req_method_type.add_argument_type(oa->object->type);
				for (size_t i = 0; i < arguments.size(); ++i) {
					Value* a = arguments[i];
					a->reanalyse(analyser, res_method_type.argument_type(i + 1));
					new_req_method_type.add_argument_type(a->type);
				}

				// if better redo
				if (new_req_method_type != req_method_type) {
					req_method_type = new_req_method_type;
				} else {
					break;
				}
			}

			oa->type = res_method_type; // only for debug

		}

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

		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);

		if (vv && vv->name == "ls") {
			Type method_type = req_method_type;

			if (methods.size() != 1 || !res_method_type.is_pure()) {
				methods = analyser->get_method("ls", oa->field->content, method_type, &res_method_type);
				if (methods.empty()) {
					add_error(analyser, SemanticException::METHOD_NOT_FOUND);
					return;
				}
			}
			method_type = res_method_type;

			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->finalize(analyser, method_type.argument_type(i + 1));
				method_type.set_argument_type(i, a->type);

				if (methods.size() != 1 || !res_method_type.is_pure()) {
					methods = analyser->get_method("ls", oa->field->content, method_type, &res_method_type);
					if (methods.empty()) {
						add_error(analyser, SemanticException::METHOD_NOT_FOUND);
						return;
					}
					method_type = res_method_type;
				}
			}

			method_type.make_it_pure();
			res_method_type = method_type;
			oa->type = method_type; // only for debug

			if (!Type::intersection(type, method_type.return_type().image_conversion(), &type)) {
				add_error(analyser, SemanticException::METHOD_NOT_FOUND);
			}
			type.make_it_pure();

			assert(method_type.is_pure() || !analyser->errors.empty());

		} else {

			oa->object->finalize(analyser, Type::UNKNOWN);
			Type method_type = Type::FUNCTION;
			method_type.set_return_type(type.fiber_conversion());
			method_type.add_argument_type(oa->object->type);
			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				method_type.add_argument_type(a->type);
			}
			if (methods.size() != 1 || !res_method_type.is_pure()) {
				methods = analyser->get_method(req_method_type.argument_type(0).get_raw_type()->clazz(), oa->field->content, method_type, &res_method_type);
				if (methods.empty()) {
					add_error(analyser, SemanticException::METHOD_NOT_FOUND);
					return;
				}
				method_type = res_method_type;
			}

			for (size_t i = 0; i < arguments.size(); ++i) {
				Value* a = arguments[i];
				a->finalize(analyser, method_type.argument_type(i + 1));
				method_type.set_argument_type(i + 1, a->type);

				if (methods.size() != 1 || !res_method_type.is_pure()) {
					methods = analyser->get_method(req_method_type.argument_type(0).get_raw_type()->clazz(), oa->field->content, method_type, &res_method_type);
					if (methods.empty()) {
						add_error(analyser, SemanticException::METHOD_NOT_FOUND);
						return;
					}
					method_type = res_method_type;
				}
			}

			method_type.make_it_pure();
			res_method_type = method_type;
			oa->type = method_type; // only for debug

			if (!Type::intersection(type, method_type.return_type().image_conversion(), &type)) {
				add_error(analyser, SemanticException::METHOD_NOT_FOUND);
			}
			type.make_it_pure();

			assert(method_type.is_pure() || !analyser->errors.empty());
		}
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
		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);

		if (vv && vv->name == "ls") {

			vector<jit_value_t> args;
			vector<jit_type_t> args_types;
			for (size_t i = 0; i < arguments.size(); ++i) {
				args.push_back(arguments[i]->compile(c));
				args_types.push_back(arguments[i]->type.jit_type());
			}
			Method* m = methods[0];
			return jit_general::convert(c.F, m->compile(c, res_method_type, args), res_method_type.return_type(), type);


		} else {

			vector<jit_value_t> args;
			vector<jit_type_t> args_types;
			args.push_back(oa->object->compile(c));
			args_types.push_back(oa->object->type.jit_type());
			for (size_t i = 0; i < arguments.size(); ++i) {
				args.push_back(arguments[i]->compile(c));
				args_types.push_back(arguments[i]->type.jit_type());
			}
			Method* m = methods[0];
			return jit_general::convert(c.F, m->compile(c, res_method_type, args), res_method_type.return_type(), type);
		}
	} else {


		jit_value_t fun = function->compile(c);
		jit_value_t res = nullptr;

		if (function->type.return_type() != Type::VOID) {
			res = jit_value_create(c.F, function->type.return_type().jit_type());
		}

		jit_label_t problem = jit_label_undefined;
		if (fun) jit_insn_branch_if_not(c.F, fun, &problem);

		vector<jit_value_t> args;
		vector<jit_type_t> args_types;

		for (size_t i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
			args_types.push_back(function->type.argument_type(i).jit_type());
		}


		jit_value_t val = nullptr;
		if (fun) {
			jit_type_t jit_return_type = function->type.return_type().jit_type();
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_return_type, args_types.data(), arguments.size(), 0);

			val = jit_insn_call_indirect(c.F, fun, sig, args.data(), arguments.size(), 0);
		} else {
			// recursive function
			val = jit_insn_call(c.F, "", c.F, nullptr, args.data(), arguments.size(), 0);
		}

		if (function->type.return_type() != Type::VOID) {
			jit_insn_store(c.F, res, val);
		}

		jit_label_t exit = jit_label_undefined;
		jit_insn_branch(c.F, &exit);
		jit_insn_label(c.F, &problem);
		if (function->type.return_type() != Type::VOID) {
			jit_insn_store(c.F, res, jit_general::constant_default(c.F, function->type.return_type()));
		}
		jit_insn_label(c.F, &exit);

		// Custom function call : 1 op
		VM::inc_ops(c.F, 1);

		return jit_general::convert(c.F, res, function->type.return_type(), type);
	}
}

}
