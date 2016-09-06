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

void FunctionCall::preanalyse(SemanticAnalyser* analyser)
{
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {
		oa->object->preanalyse(analyser);
		if (!oa->object->isLeftValue()) {
			oa->object->add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
			return;
		}

		vector<Type> args_types;
		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->preanalyse(analyser);
			args_types.push_back(arguments[i]->type);
		}

		methods = analyser->get_method(oa->field->content, Type::UNKNOWN, oa->object->type, args_types);

		if (methods.empty()) {
			add_error(analyser, SemanticException::METHOD_NOT_FOUND);
			return;
		}

		Type generic_method = methods[0].type;
		for (size_t i = 1; i < methods.size(); ++i) {
			generic_method = Type::union_of(generic_method, methods[i].type);
		}

		oa->type = generic_method; // only for debug

		((LeftValue*) oa->object)->will_take(analyser, generic_method.argument_type(0));
		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->will_require(analyser, generic_method.argument_type(i + 1));
		}
		type = generic_method.return_type().image_conversion();
	} else {
		assert(0);
	}
}

void FunctionCall::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {
		// TODO trier methods et garder que celle dont le type de retour peu être converti en req_type
		// recalculer le type generic
		// relancer will_require et will_take
	} else {
		assert(0);
	}
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		type.make_it_complete();

		// TODO faire comme c'est prévu dans will_require
		oa->object->analyse(analyser, Type::UNKNOWN);

		vector<Type> args_types;
		for (Value* a : arguments) {
			args_types.push_back(a->type);
		}

		methods = analyser->get_method(oa->field->content, type.fiber_conversion(), oa->object->type, args_types);
		if (methods.empty()) {
			add_error(analyser, SemanticException::METHOD_NOT_FOUND);
			return;
		}
		// take the first one
		methods.resize(1);

		oa->type = methods[0].type; // only for debug

		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->analyse(analyser, methods[0].type.argument_type(i + 1));
		}

	} else {
		assert(0);
	}

	/*
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
	*/
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

}
