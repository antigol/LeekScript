#include "ObjectAccess.hpp"
#include <string>

using namespace std;

namespace ls {

ObjectAccess::ObjectAccess() {
	/*
	field = nullptr;
	object = nullptr;
	type = Type::POINTER;
	class_attr = false;
	attr_addr = nullptr;
	*/
}

ObjectAccess::~ObjectAccess() {
	delete object;
//	delete field_string;
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

void ObjectAccess::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
	try {
		ulong index = stoul(field->content);
		object->preanalyse(analyser);

		if (object->type.raw_type == &RawType::TUPLE && index < object->type.elements_types.size()) {
			type = object->type.elements_types[index];
		} else {
			type = Type::VOID;
		}

	} catch (exception&) {
		type = Type::VOID;
	}
}

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
	try {
		ulong index = stoul(field->content);

		if (object->type.raw_type == &RawType::TUPLE && index < object->type.elements_types.size()) {
			if (!Type::intersection(type, req_type, &type)) {
				stringstream oss;
				print(oss, 0, false);
				analyser->add_error({ SemanticException::TYPE_MISMATCH, line(), oss.str() });
			}
			Type tuple_type = Type::TUPLE;
			tuple_type.elements_types.resize(object->type.elements_types.size(), Type::UNKNOWN);
			tuple_type.elements_types[index] = type;

			object->analyse(analyser, tuple_type);

			type = object->type.elements_types[index];
		} else {
			type = Type::VOID;
		}

	} catch (exception&) {
		type = Type::VOID;
	}
}

jit_value_t ObjectAccess::compile(Compiler& c) const
{
	try {
		ulong index = stoul(field->content);

		if (object->type.raw_type == &RawType::TUPLE && index < object->type.elements_types.size()) {
			jit_value_t ptr = jit_insn_address_of(c.F, object->compile(c));
			jit_type_t obj_type = object->type.jit_type();
			return jit_insn_load_relative(c.F, ptr, jit_type_get_offset(obj_type, index), object->type.element_type(index).jit_type());
		}

	} catch (exception&) {
	}
	return nullptr;
}

jit_value_t ObjectAccess::compile_l(Compiler& c) const
{
	try {
		ulong index = stoul(field->content);

		if (object->type.raw_type == &RawType::TUPLE && index < object->type.elements_types.size()) {
			jit_value_t ptr = jit_insn_address_of(c.F, object->compile(c));
			jit_type_t obj_type = object->type.jit_type();
			return jit_insn_add_relative(c.F, ptr, jit_type_get_offset(obj_type, index));
		}

	} catch (exception&) {
	}
	return nullptr;
}

}
