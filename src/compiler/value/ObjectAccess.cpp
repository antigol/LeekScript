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

bool ObjectAccess::isLeftValue() const
{
	return object->isLeftValue();
}

// DONE 2
void ObjectAccess::analyse_help(SemanticAnalyser* analyser)
{
	object->analyse(analyser);
	left_type = Type::UNKNOWN;
	type = Type::UNKNOWN;
}

void ObjectAccess::reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type)
{
	if (!Type::intersection(left_type, req_left_type, &left_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (object->type.raw_type == &RawType::TUPLE) {
		ulong index;
		try {
			index = stoul(field->content);
		} catch (exception&) {
			add_error(analyser, SemanticException::NO_ATTRIBUTE_WITH_THIS_NAME);
			return;
		}
		if (index >= object->type.elements_types.size()) {
			add_error(analyser, SemanticException::NO_ATTRIBUTE_WITH_THIS_NAME);
		}

		Type req_type = object->type;
		req_type.set_element_type(index, type);
		Type req_left_type = object->type;
		req_left_type.set_element_type(index, left_type);

		if (isLeftValue()) {
			LeftValue* left = dynamic_cast<LeftValue*>(object);
			left->reanalyse_l(analyser, req_type, req_left_type);
			if (!Type::intersection(left_type, left->left_type.element_type(index), &left_type)) {
				add_error(analyser, SemanticException::TYPE_MISMATCH);
			}
		} else {
			object->reanalyse(analyser, req_type);
		}
		if (!Type::intersection(type, object->type.element_type(index).image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else {
		object->reanalyse(analyser, Type::UNKNOWN);
	}
}

void ObjectAccess::finalize_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type)
{
	if (!Type::intersection(left_type, req_left_type, &left_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (isLeftValue()) {
		LeftValue* left = dynamic_cast<LeftValue*>(object);
		left->finalize_l(analyser, Type::UNKNOWN, Type::UNKNOWN);
	} else {
		object->finalize(analyser, Type::UNKNOWN);
	}

	if (object->type.raw_type == &RawType::TUPLE) {
		ulong index;
		try {
			index = stoul(field->content);
		} catch (exception&) {
			add_error(analyser, SemanticException::NO_ATTRIBUTE_WITH_THIS_NAME);
			return;
		}
		if (index >= object->type.elements_types.size()) {
			add_error(analyser, SemanticException::NO_ATTRIBUTE_WITH_THIS_NAME);
		}

		if (isLeftValue()) {
			LeftValue* left = dynamic_cast<LeftValue*>(object);
			left_type = left->left_type.element_type(index);
		}
		if (!Type::intersection(type, object->type.element_type(index).image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();
	} else {
		add_error(analyser, SemanticException::NO_ATTRIBUTE_WITH_THIS_NAME);
	}
}

jit_value_t ObjectAccess::compile(Compiler& c) const
{
	if (object->type.raw_type == &RawType::TUPLE) {
		ulong index = stoul(field->content);

		jit_value_t ptr = jit_insn_address_of(c.F, object->compile(c));
		jit_type_t ty = object->type.jit_type();
		return jit_insn_load_relative(c.F, ptr, jit_type_get_offset(ty, index), object->type.element_type(index).jit_type());

	} else {
		assert(0);
	}
	return nullptr;
}

jit_value_t ObjectAccess::compile_l(Compiler& c) const
{
	if (object->type.raw_type == &RawType::TUPLE) {
		ulong index = stoul(field->content);

		LeftValue* left = dynamic_cast<LeftValue*>(object);

		jit_value_t ptr = left->compile_l(c);
		jit_type_t ty = object->type.jit_type();
		return jit_insn_add_relative(c.F, ptr, jit_type_get_offset(ty, index));

	} else {
		assert(0);
	}
	return nullptr;
}

}
