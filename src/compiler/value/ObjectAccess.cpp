#include "ObjectAccess.hpp"

#include <chrono>

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"

using namespace std;

namespace ls {

ObjectAccess::ObjectAccess() {
	field = nullptr;
	object = nullptr;
	type = Type::POINTER;
	class_attr = false;
	attr_addr = nullptr;
}

ObjectAccess::~ObjectAccess() {
	delete object;
	delete field_string;
}

void ObjectAccess::print(ostream& os, int indent, bool debug) const {
	object->print(os, indent, debug);
	os << "." << field->content;
	if (debug) {
		os << " " << type;
	}
}

unsigned ObjectAccess::line() const {
	return 0;
}

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (field_string == nullptr) {
		field_string = new LSString(field->content);
	}

	object->analyse(analyser, Type::UNKNOWN);

	// Get the object class : 12 => Number
	object_class_name = object->type.clazz;
	LSClass* object_class = nullptr;
	if (analyser->program->system_vars.find(object_class_name) != analyser->program->system_vars.end()) {
		object_class = (LSClass*) analyser->program->system_vars[object_class_name];
	}

	// Static attribute? (Number.PI <= static attr)
	VariableValue* vv = dynamic_cast<VariableValue*>(object);

	if (object->type.raw_type == RawType::CLASS and vv != nullptr) {

		class_name = vv->name;
		LSClass* std_class = (LSClass*) analyser->program->system_vars[class_name];

		ModuleStaticField& mod_field = std_class->static_fields[field->content];
		type = mod_field.type;

		if (mod_field.fun != nullptr) {
			access_function = mod_field.fun;
		}
		field_type = mod_field.type;
	}

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (object_class != nullptr) {

		// Object attribute
		bool is_field = false;
		try {
			type = object_class->fields.at(field->content);
			is_field = true;
			class_attr = true;
		} catch (exception& e) {}

		// Otherwise search in class methods
		if (not is_field) {
			try {
				type = object_class->methods.at(field->content)[0].type;
				class_attr = true;
			} catch (exception& e) {}
		}
	}

	if (not access_function and not class_attr) {
		object->analyse(analyser, Type::POINTER);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}

//	cout << "object_access '" << field->content << "' type : " << type << endl;
}

void ObjectAccess::change_type(SemanticAnalyser*, const Type& req_type) {
	type = req_type;
}

LSValue* object_access(LSValue* o, LSString* k) {
	return o->attr(k);
}

LSValue** object_access_l(LSValue* o, LSString* k) {
	return o->attrL(k);
}

Compiler::value ObjectAccess::compile(Compiler& c) const {

	// Special case for custom attributes, accessible via a function
	if (access_function != nullptr) {

		auto fun = (jit_value_t (*)(jit_function_t)) access_function;
		jit_value_t res = fun(c.F);

		if (field_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res, type), type};
		}
		return {res, type};
	}

	if (class_attr) {

		// TODO : only functions!
		return {LS_CREATE_POINTER(c.F, new LSFunction(attr_addr)), type};

	} else {

		auto o = object->compile(c);

		jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

		jit_value_t k = LS_CREATE_POINTER(c.F, field_string);
		jit_value_t args[] = {o.v, k};

		jit_value_t res = jit_insn_call_native(c.F, "access", (void*) object_access, sig, args, 2, JIT_CALL_NOTHROW);

		VM::delete_temporary(c.F, o.v);
		return {res, type};
	}
}

Compiler::value ObjectAccess::compile_l(Compiler& c) const {

	auto o = object->compile(c);

	jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 2, 0);

	jit_value_t k = LS_CREATE_POINTER(c.F, field_string);
	jit_value_t args[] = {o.v, k};

	jit_value_t res = jit_insn_call_native(c.F, "access_l", (void*) object_access_l, sig, args, 2, JIT_CALL_NOTHROW);

	return {res, type};
}

}
