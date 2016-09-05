#include "FunctionCall.hpp"
#include "ObjectAccess.hpp"

using namespace std;

namespace ls {

FunctionCall::FunctionCall() : method(Type::VOID, nullptr) {
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

void FunctionCall::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
	constant = false;
	method = Method(Type::VOID, nullptr);

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

		// Convertion
		type = method.type.return_type().image_conversion();
		if (!Type::intersection(type, req_type, &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_complete();
		return;
	}

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

	// Convertion
	type = function->type.return_type().image_conversion();
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_complete();
}

jit_value_t FunctionCall::compile(Compiler& c) const
{
	if (method.addr) {
		ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);

		vector<jit_value_t> args;
		vector<jit_type_t> args_types;
		args.push_back(oa->object->compile(c));
		args_types.push_back(oa->object->type.jit_type());
		for (size_t i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
			args_types.push_back(arguments[i]->type.jit_type());
		}
		Type rt = method.type.return_type();
		return Compiler::compile_convert(c.F, Compiler::call_native(c.F, rt.jit_type(), args_types, method.addr, args), rt, type);
	}


	/*
	 * Default function
	 */
	jit_value_t fun = function->compile(c);
	jit_value_t res = function->type.return_type() != Type::VOID ? jit_value_create(c.F, function->type.return_type().jit_type()) : nullptr;

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
		args_types.push_back(function->type.argument_type(i).jit_type());

		if (function->type.argument_type(i).must_manage_memory()) {
			args[i] = VM::move_inc_obj(c.F, args[i]);
		}
	}

	jit_type_t jit_return_type = function->type.return_type().jit_type();

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
