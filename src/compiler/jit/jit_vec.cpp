#include "jit_vec.hpp"
#include "../../vm/value/LSVar.hpp"

using namespace std;
using namespace ls;

vector<char>* jit_vec_create() {
	return new vector<char>();
}

void jit_vec_enlarge(vector<char>* vec, uint32_t size) {
	vec->resize(vec->size() + size);
}

char* jit_vec_begin(vector<char>* vec) {
	return vec->data();
}

char* jit_vec_end(vector<char>* vec) {
	return vec->data() + vec->size();
}

uint32_t jit_vec_size(vector<char>* vec) {
	return vec->size();
}

void jit_vec_delete(vector<char>* vec) {
	delete vec;
}

jit_type_t jit_vec::jit_type()
{
	jit_type_t fields[] = { jit_type_void_ptr, jit_type_int };
	return jit_type_create_struct(fields, 2, 0);
}

jit_value_t jit_vec::create(jit_function_t F)
{
	jit_type_t type = jit_type();
	jit_value_t val = jit_value_create(F, type);
	jit_value_t ptr = jit_insn_address_of(F, val);

	jit_value_t vec = jit_general::call_native(F, jit_type_void_ptr, { }, (void*) jit_vec_create, { });

	jit_insn_store_relative(F, ptr, 0, vec);
	jit_insn_store_relative(F, ptr, jit_type_get_offset(type, 1), jit_value_create_nint_constant(F, jit_type_int, 0));

	jit_type_free(type);
	return val;
}

void jit_vec::push_move_inc(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t value)
{
	jit_value_t element_size = jit_value_create_nint_constant(F, jit_type_uint, element_type.bytes());
	jit_value_t vec = jit_insn_load_relative(F, jit_insn_address_of(F, array), 0, LS_POINTER);

	jit_general::call_native(F, jit_type_void, { jit_type_void_ptr, jit_type_uint }, (void*) jit_vec_enlarge, { vec, element_size });

	value = jit_general::move_inc(F, value, element_type);
	jit_value_t end = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end, { vec });

	jit_insn_store_relative(F, end, -element_type.bytes(), value);
}

jit_value_t jit_vec::index(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index)
{
	jit_value_t vec = jit_insn_load_relative(F, jit_insn_address_of(F, array), 0, LS_POINTER);


	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });
	jit_type_t jit_elem_type = element_type.jit_type();

	jit_value_t elem_ptr = jit_insn_load_elem_address(F, begin, jit_insn_convert(F, index, jit_type_uint, 0), jit_elem_type);
	jit_value_t res = jit_value_create(F, jit_elem_type);

	jit_label_t l1 = jit_label_undefined;
	jit_label_t l2 = jit_label_undefined;
	jit_insn_branch_if(F, jit_insn_lt(F, elem_ptr, end), &l1);

	jit_insn_store(F, res, jit_general::constant_default(F, element_type));
	jit_insn_branch(F, &l2);

	jit_insn_label(F, &l1);
	jit_insn_store(F, res, jit_insn_load_relative(F, elem_ptr, 0, jit_elem_type));

	jit_insn_label(F, &l2);
	jit_type_free(jit_elem_type);
	return jit_general::move(F, res, element_type);
}

jit_value_t jit_vec::index_delete_temporary(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index)
{
	jit_type_t type = jit_type();
	jit_type_t jit_elem_type = element_type.jit_type();

	jit_value_t ptr  = jit_insn_address_of(F, array);
	jit_value_t vec  = jit_insn_load_relative(F, ptr, 0, jit_type_void_ptr);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(type, 1), jit_type_int);


	// Index

	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });

	jit_value_t elem_ptr = jit_insn_load_elem_address(F, begin, jit_insn_convert(F, index, jit_type_uint, 0), jit_elem_type);
	jit_value_t res = jit_value_create(F, jit_elem_type);

	jit_label_t good_index = jit_label_undefined;
	jit_label_t end_index = jit_label_undefined;
	jit_insn_branch_if(F, jit_insn_lt(F, elem_ptr, end), &good_index);

	jit_insn_store(F, res, jit_general::constant_default(F, element_type));
	jit_insn_branch(F, &end_index);

	jit_insn_label(F, &good_index);
	jit_insn_store(F, res, jit_insn_load_relative(F, elem_ptr, 0, jit_elem_type));

	jit_insn_label(F, &end_index);

	// Delete temporary

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if(F, refs, &exit);

	if (element_type.must_manage_memory()) {

		jit_label_t stop = jit_label_undefined;
		jit_label_t loop = jit_label_undefined;
		jit_label_t increment = jit_label_undefined;
		jit_insn_label(F, &loop);
		jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);
		jit_insn_branch_if(F, jit_insn_eq(F, begin, elem_ptr), &increment);

		jit_general::delete_ref(F, jit_insn_load_relative(F, begin, 0, jit_elem_type), element_type);

		jit_insn_label(F, &increment);
		jit_insn_store(F, begin, jit_insn_add_relative(F, begin, jit_type_get_size(jit_elem_type)));

		jit_insn_branch(F, &loop);
		jit_insn_label(F, &stop);

	}

	jit_general::call_native(F, jit_type_void, { jit_type_void_ptr }, (void*) jit_vec_delete, { vec });

	jit_insn_label(F, &exit);




	jit_type_free(type);
	jit_type_free(jit_elem_type);
	return res;
}

jit_value_t jit_vec::index_l(jit_function_t F, const Type& element_type, jit_value_t array, jit_value_t index)
{
	jit_value_t vec = jit_insn_load_relative(F, jit_insn_address_of(F, array), 0, LS_POINTER);

	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });
	jit_type_t jit_elem_type = element_type.jit_type();

	jit_value_t elem_ptr = jit_insn_load_elem_address(F, begin, jit_insn_convert(F, index, jit_type_uint, 0), jit_elem_type);

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if(F, jit_insn_lt(F, elem_ptr, end), &exit);

	jit_insn_store(F, elem_ptr, jit_general::constant_ptr(F, nullptr));

	jit_insn_label(F, &exit);
	jit_type_free(jit_elem_type);
	return elem_ptr;
}

jit_value_t jit_vec::size(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_value_t array_ptr = jit_insn_address_of(F, array);
	jit_value_t vec = jit_insn_load_relative(F, array_ptr, 0, LS_POINTER);

	jit_value_t size = jit_general::call_native(F, jit_type_uint, { jit_type_void_ptr }, (void*) jit_vec_size, { vec });
	return jit_insn_div(F, size, jit_general::constant_i32(F, element_type.bytes()));
}

void jit_vec::delete_ref(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_type_t type = jit_type();

	jit_value_t ptr = jit_insn_address_of(F, array);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(type, 1), jit_type_int);

	jit_label_t l1 = jit_label_undefined;

	jit_insn_branch_if_not(F, refs, &l1);

	jit_insn_store_relative(F, ptr, jit_type_get_offset(type, 1), jit_insn_sub(F, refs, jit_general::constant_i32(F, 1)));

	delete_temporary(F, element_type, array);

	jit_insn_label(F, &l1);
	jit_type_free(type);
}

void jit_vec::delete_temporary(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_type_t type = jit_type();

	jit_value_t ptr  = jit_insn_address_of(F, array);
	jit_value_t vec  = jit_insn_load_relative(F, ptr, 0, jit_type_void_ptr);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(type, 1), jit_type_int);

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if(F, refs, &exit);

	if (element_type.must_manage_memory()) {
		jit_type_t jit_element_type = element_type.jit_type();

		jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
		jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });

		jit_label_t stop = jit_label_undefined;
		jit_label_t loop = jit_label_undefined;
		jit_insn_label(F, &loop);
		jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

		jit_general::delete_ref(F, jit_insn_load_relative(F, begin, 0, jit_element_type), element_type);

		jit_insn_store(F, begin, jit_insn_add_relative(F, begin, jit_type_get_size(jit_element_type)));

		jit_insn_branch(F, &loop);
		jit_insn_label(F, &stop);

		jit_type_free(jit_element_type);
	}

	jit_general::call_native(F, jit_type_void, { jit_type_void_ptr }, (void*) jit_vec_delete, { vec });

	jit_insn_label(F, &exit);
	jit_type_free(type);
}

jit_value_t jit_vec::move(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_type_t type = jit_vec::jit_type();
	jit_type_t jit_element_type = element_type.jit_type();

	jit_value_t ptr  = jit_insn_address_of(F, array);
	jit_value_t vec  = jit_insn_load_relative(F, ptr, 0, LS_POINTER);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(type, 1), jit_type_int);

	jit_value_t copy = jit_value_create(F, type);
	jit_insn_store(F, copy, array);

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if_not(F, refs, &exit);

	jit_insn_store(F, copy, jit_vec::create(F));

	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });

	jit_label_t stop = jit_label_undefined;
	jit_label_t loop = jit_label_undefined;
	jit_insn_label(F, &loop);
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_vec::push_move_inc(F, element_type, copy, jit_insn_load_relative(F, begin, 0, jit_element_type));

	jit_insn_store(F, begin, jit_insn_add_relative(F, begin, jit_type_get_size(jit_element_type)));

	jit_insn_branch(F, &loop);
	jit_insn_label(F, &stop);

	jit_insn_label(F, &exit);
	jit_type_free(type);
	jit_type_free(jit_element_type);
	return copy;
}

jit_value_t jit_vec::move_inc(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_value_t copy = move(F, element_type, array);
	inc_refs(F, copy);
	return copy;
}

void jit_vec::inc_refs(jit_function_t F, jit_value_t array)
{
	jit_type_t type = jit_vec::jit_type();

	jit_value_t ptr = jit_insn_address_of(F, array);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(type, 1), jit_type_int);

	jit_insn_store_relative(F, ptr, jit_type_get_offset(type, 1), jit_insn_add(F, refs, jit_general::constant_i32(F, 1)));

	jit_type_free(type);
}

static void jit_vec_print_open() { cout << "["; }
static void jit_vec_print_close() { cout << "]"; }
static void jit_vec_print_comma() { cout << ", "; }

void jit_vec::print(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_type_t type = jit_vec::jit_type();
	jit_type_t jit_element_type = element_type.jit_type();

	jit_value_t vec  = jit_insn_load_relative(F, jit_insn_address_of(F, array), 0, LS_POINTER);

	jit_general::call_native(F, LS_VOID, { }, (void*) jit_vec_print_open, { });

	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });

	jit_label_t stop = jit_label_undefined;
	jit_label_t loop = jit_label_undefined;
	jit_insn_label(F, &loop);
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_value_t elem = jit_insn_load_relative(F, begin, 0, jit_element_type);
	jit_general::print(F, elem, element_type);

	jit_insn_store(F, begin, jit_insn_add_relative(F, begin, jit_type_get_size(jit_element_type)));
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_general::call_native(F, LS_VOID, { }, (void*) jit_vec_print_comma, { });

	jit_insn_branch(F, &loop);
	jit_insn_label(F, &stop);

	jit_general::call_native(F, LS_VOID, { }, (void*) jit_vec_print_close, { });

	jit_type_free(type);
	jit_type_free(jit_element_type);
}

static LSVar* jit_vec_string_open() { return new LSVar("["); }
static void jit_vec_string_close(LSVar* value) { value->text += "]"; }
static void jit_vec_string_comma(LSVar* value) { value->text += ", "; }
static void jit_vec_string_append(LSVar* value, LSVar* x) { value->text += x->text; delete x; }

jit_value_t jit_vec::string(jit_function_t F, const Type& element_type, jit_value_t array)
{
	jit_type_t type = jit_vec::jit_type();
	jit_type_t jit_element_type = element_type.jit_type();

	jit_value_t vec  = jit_insn_load_relative(F, jit_insn_address_of(F, array), 0, LS_POINTER);

	jit_value_t res = jit_general::call_native(F, LS_POINTER, { }, (void*) jit_vec_string_open, { });

	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_begin, { vec });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_vec_end,   { vec });

	jit_label_t stop = jit_label_undefined;
	jit_label_t loop = jit_label_undefined;
	jit_insn_label(F, &loop);
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_value_t elem = jit_insn_load_relative(F, begin, 0, jit_element_type);
	jit_value_t str = jit_general::string(F, elem, element_type);
	jit_general::call_native(F, LS_VOID, { LS_POINTER, LS_POINTER }, (void*) jit_vec_string_append, { res, str });

	jit_insn_store(F, begin, jit_insn_add_relative(F, begin, jit_type_get_size(jit_element_type)));
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_vec_string_comma, { res });

	jit_insn_branch(F, &loop);
	jit_insn_label(F, &stop);

	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_vec_string_close, { res });

	jit_type_free(type);
	jit_type_free(jit_element_type);

	return res;
}
