#include "ObjectAccess.hpp"
#include <chrono>
#include "../semantic/SemanticAnalyser.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"

using namespace std;

namespace ls {

ObjectAccess::ObjectAccess(std::shared_ptr<Token> token) : field(token) {
	object = nullptr;
	type = Type::POINTER;
	attr_addr = nullptr;
}

ObjectAccess::~ObjectAccess() {
	delete object;
}

void ObjectAccess::print(ostream& os, int indent, bool debug) const {
	object->print(os, indent, debug);
	os << "." << field->content;
	if (debug) {
		os << " " << type;
	}
}

Location ObjectAccess::location() const {
	return {object->location().start, field->location.end};
}

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	object->analyse(analyser, Type::UNKNOWN);

	// Get the object class : 12 => Number
	object_class_name = object->type.clazz;
	LSClass* object_class = nullptr;
	if (analyser->vm->system_vars.find(object_class_name) != analyser->vm->system_vars.end()) {
		object_class = (LSClass*) analyser->vm->system_vars[object_class_name];
	}

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object);

	bool found = false;
	if (object->type.raw_type == RawType::CLASS and vv != nullptr) {

		auto std_class = (LSClass*) analyser->vm->system_vars.at(vv->name);

		if (std_class->static_fields.find(field->content) != std_class->static_fields.end()) {

			auto mod_field = std_class->static_fields.at(field->content);
			type = mod_field.type;

			if (mod_field.fun != nullptr) {
				static_access_function = mod_field.fun;
			}
			field_type = mod_field.type;
			found = true;
		}
	}

	auto value_class = (LSClass*) analyser->vm->system_vars.at("Value");

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (!found and object_class != nullptr) {
		// Attribute : (x -> x).return
		try {
			auto f = object_class->fields.at(field->content);
			type = f.type;
		} catch (...) {
			// Attribute in Value?
			try {
				auto f = value_class->fields.at(field->content);
				type = f.type;
				class_field = true;
				if (f.fun != nullptr) {
					access_function = f.fun;
				}
			} catch (...) {
				// Method : 12.abs
				try {
					Method f = object_class->methods.at(field->content)[0];
					type = f.type;
					attr_addr = f.addr;
					class_method = true;
				} catch (...) {
					try {
						Method f = value_class->methods.at(field->content)[0];
						type = f.type;
						attr_addr = f.addr;
						class_field = true;
					} catch (...) {
						if (object_class->name != "Object") {
							if (object->type.raw_type == RawType::CLASS and vv != nullptr) {
								analyser->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, vv->name}});
							} else {
								analyser->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, object_class->name}});
							}
							return;
						}
					}
				}
			}
		}
	}

	if (not access_function and not static_access_function and not class_method) {
		object->analyse(analyser, Type::POINTER);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

void ObjectAccess::change_type(SemanticAnalyser*, const Type&) {}

Compiler::value ObjectAccess::compile(Compiler& c) const {

	// Special case for custom attributes, accessible via a function
	// Static attributes : Number.PI
	if (static_access_function != nullptr) {

		auto fun = (Compiler::value (*)(Compiler&)) static_access_function;
		Compiler::value res = fun(c);

		if (field_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, field_type), type};
		}
		return res;
	}

	// Field with an access function : 12.class
	if (access_function != nullptr) {
		auto obj = object->compile(c);
		object->compile_end(c);
		auto fun = (Compiler::value (*)(Compiler&, Compiler::value)) access_function;
		return fun(c, obj);
	}

	// Class method : 12.abs
	if (class_method) {
		return c.new_pointer(new LSFunction<LSValue*>(attr_addr));
	}

	// Default : object.attr
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_pointer(&field->content);
	auto r = c.insn_call(type, {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attr(*key);
	});
	c.insn_delete_temporary(o);
	return r;
}

Compiler::value ObjectAccess::compile_l(Compiler& c) const {
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_pointer(&field->content);
	auto r = c.insn_call(type, {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attrL(*key);
	});
	return r;
}

Value* ObjectAccess::clone() const {
	auto oa = new ObjectAccess(field);
	oa->object = object->clone();
	return oa;
}

}
