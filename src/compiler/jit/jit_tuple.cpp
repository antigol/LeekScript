#include "jit_tuple.hpp"
#include "../../vm/value/LSVar.hpp"
#include <cassert>

using namespace std;
using namespace ls;

jit_type_t jit_tuple::jit_type(const Type& type)
{
	vector<jit_type_t> fields;
	for (const Type& x : type.elements_types) {
		fields.push_back(x.jit_type());
	}
	return jit_type_create_struct(fields.data(), fields.size(), 0);
}

jit_value_t jit_tuple::eq(jit_function_t F, jit_value_t v1, jit_value_t v2, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t res = jit_value_create(F, LS_BOOLEAN);
	jit_insn_store(F, res, jit_general::constant_bool(F, false));

	jit_type_t ty = type.jit_type();
	jit_label_t end = jit_label_undefined;

	jit_value_t ptr1 = jit_insn_address_of(F, v1);
	jit_value_t ptr2 = jit_insn_address_of(F, v2);
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		const Type& el_type = type.elements_types[i];
		jit_nint offset = jit_type_get_offset(ty, i);
		jit_value_t el1 = jit_insn_load_relative(F, ptr1, offset, el_type.jit_type());
		jit_value_t el2 = jit_insn_load_relative(F, ptr2, offset, el_type.jit_type());
		jit_value_t eq = jit_general::eq(F, el1, el_type, el2, el_type);
		jit_insn_branch_if_not(F, eq, &end);
	}
	jit_insn_store(F, res, jit_general::constant_bool(F, true));
	jit_insn_label(F, &end);
	jit_type_free(ty);
	return res;
}

jit_value_t jit_tuple::lt(jit_function_t F, jit_value_t v1, jit_value_t v2, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t res = jit_value_create(F, LS_BOOLEAN);
	jit_insn_store(F, res, jit_general::constant_bool(F, true));

	jit_type_t ty = type.jit_type();
	jit_label_t vrai = jit_label_undefined;
	jit_label_t faux = jit_label_undefined;

	jit_value_t ptr1 = jit_insn_address_of(F, v1);
	jit_value_t ptr2 = jit_insn_address_of(F, v2);
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		const Type& el_type = type.elements_types[i];
		jit_nint offset = jit_type_get_offset(ty, i);
		jit_value_t el1 = jit_insn_load_relative(F, ptr1, offset, el_type.jit_type());
		jit_value_t el2 = jit_insn_load_relative(F, ptr2, offset, el_type.jit_type());
		jit_value_t lt = jit_general::lt(F, el1, el_type, el2, el_type);
		jit_insn_branch_if(F, lt, &vrai);
		jit_value_t eq = jit_general::eq(F, el1, el_type, el2, el_type);
		jit_insn_branch_if_not(F, eq, &faux);
	}
	jit_insn_label(F, &faux);
	jit_insn_store(F, res, jit_general::constant_bool(F, false));
	jit_insn_label(F, &vrai);
	return res;
}

void jit_tuple::delete_ref(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t ptr = jit_insn_address_of(F, v);
	jit_type_t jit_type = type.jit_type();
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_value_t el = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, i), type.element_type(i).jit_type());
		jit_general::delete_ref(F, el, type.element_type(i));
	}
	jit_type_free(jit_type);
}

void jit_tuple::delete_temporary(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t ptr = jit_insn_address_of(F, v);
	jit_type_t jit_type = type.jit_type();
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_value_t el = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, i), type.element_type(i).jit_type());
		jit_general::delete_temporary(F, el, type.element_type(i));
	}
	jit_type_free(jit_type);
}

jit_value_t jit_tuple::move_inc(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_type_t jit_type = type.jit_type();

	jit_value_t res = jit_value_create(F, jit_type);
	jit_insn_store(F, res, v);
	jit_value_t ptr = jit_insn_address_of(F, res);
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_type_t jit_elem_type = type.elements_types[i].jit_type();

		jit_nint offset = jit_type_get_offset(jit_type, i);
		jit_value_t el = jit_insn_load_relative(F, ptr, offset, jit_elem_type);
		el = jit_general::move_inc(F, el, type.elements_types[i]);
		jit_insn_store_relative(F, ptr, offset, el);

		jit_type_free(jit_elem_type);
	}
	jit_type_free(jit_type);
	return res;
}

jit_value_t jit_tuple::move(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_type_t jit_type = type.jit_type();

	jit_value_t res = jit_value_create(F, jit_type);
	jit_insn_store(F, res, v);
	jit_value_t ptr = jit_insn_address_of(F, res);
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_type_t jit_elem_type = type.elements_types[i].jit_type();

		jit_nint offset = jit_type_get_offset(jit_type, i);
		jit_value_t el = jit_insn_load_relative(F, ptr, offset, jit_elem_type);
		el = jit_general::move(F, el, type.elements_types[i]);
		jit_insn_store_relative(F, ptr, offset, el);

		jit_type_free(jit_elem_type);
	}
	jit_type_free(jit_type);
	return res;
}

jit_value_t jit_tuple::create_def(jit_function_t F, const Type& type)
{
	jit_type_t jit_type = type.jit_type();
	jit_value_t val = jit_value_create(F, jit_type);
	jit_value_t ptr = jit_insn_address_of(F, val);
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_value_t el = jit_general::constant_default(F, type.element_type(i));
		jit_insn_store_relative(F, ptr, jit_type_get_offset(jit_type, i), el);
	}
	jit_type_free(jit_type);
	return val;
}

void jit_tuple::inc_refs(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t ptr = jit_insn_address_of(F, v);
	jit_type_t jit_type = type.jit_type();
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_nint offset = jit_type_get_offset(jit_type, i);
		jit_value_t el = jit_insn_load_relative(F, ptr, offset, type.element_type(i).jit_type());
		jit_general::inc_refs(F, el, type.elements_types[i]);
	}
	jit_type_free(jit_type);
}

void jit_tuple_print_open() { cout << "("; }
void jit_tuple_print_close() { cout << ")"; }
void jit_tuple_print_comma() { cout << ", "; }

void jit_tuple::print(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t ptr = jit_insn_address_of(F, v);
	jit_type_t jit_type = type.jit_type();

	jit_general::call_native(F, LS_VOID, { }, (void*) jit_tuple_print_open, { });
	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_nint offset = jit_type_get_offset(jit_type, i);
		jit_type_t jit_elem_type = type.element_type(i).jit_type();
		jit_value_t el = jit_insn_load_relative(F, ptr, offset, jit_elem_type);

		jit_general::print(F, el, type.elements_types[i]);
		jit_type_free(jit_elem_type);

		if (i + 1 < type.elements_types.size() || type.elements_types.size() == 1)
			jit_general::call_native(F, LS_VOID, { }, (void*) jit_tuple_print_comma, { });
	}
	jit_general::call_native(F, LS_VOID, { }, (void*) jit_tuple_print_close, { });
	jit_type_free(jit_type);
}

static LSVar* jit_tuple_string_open() { return new LSVar("("); }
static void jit_tuple_string_close(LSVar* value) { value->text += ")"; }
static void jit_tuple_string_comma(LSVar* value) { value->text += ", "; }
static void jit_tuple_string_append(LSVar* value, LSVar* x) { value->text += x->text; delete x; }

jit_value_t jit_tuple::string(jit_function_t F, jit_value_t v, const Type& type)
{
	assert(type.raw_type == &RawType::TUPLE);

	jit_value_t res = jit_general::call_native(F, LS_POINTER, { }, (void*) jit_tuple_string_open, { });

	jit_value_t ptr = jit_insn_address_of(F, v);
	jit_type_t jit_type = type.jit_type();

	for (size_t i = 0; i < type.elements_types.size(); ++i) {
		jit_nint offset = jit_type_get_offset(jit_type, i);
		jit_type_t jit_elem_type = type.element_type(i).jit_type();
		jit_value_t el = jit_insn_load_relative(F, ptr, offset, jit_elem_type);

		jit_value_t str = jit_general::string(F, el, type.elements_types[i]);
		jit_general::call_native(F, LS_VOID, { LS_POINTER, LS_POINTER }, (void*) jit_tuple_string_append, { res, str });

		jit_type_free(jit_elem_type);

		if (i + 1 < type.elements_types.size() || type.elements_types.size() == 1)
			jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_tuple_string_comma, { res });
	}
	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_tuple_string_close, { res });
	jit_type_free(jit_type);
	return res;
}

