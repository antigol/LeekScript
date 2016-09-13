#include "jit_set.hpp"

using namespace std;
using namespace ls;



jit_type_t jit_set::jit_type()
{
	jit_type_t fields[] = { jit_type_void_ptr, jit_type_int };
	return jit_type_create_struct(fields, 2, 0);
}

struct jit_set_less {
	void* comp;
	bool operator() (void* lhs, void* rhs) const {
		auto fun = (int32_t (*)(void*, void*)) comp;
		return fun(lhs, rhs);
	}
};

typedef set<void*, jit_set_less> jit_set_;

static jit_set_* jit_set_create(void* f) {
	return new jit_set_(jit_set_less { f });
}

jit_value_t jit_set::create(jit_function_t F, const Type& value_type)
{
	jit_type_t type = jit_type();
	jit_value_t val = jit_value_create(F, type);
	jit_value_t ptr = jit_insn_address_of(F, val);

	void* f = jit_general::closure_lt(value_type);

	jit_value_t vec = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_set_create, { jit_general::constant_ptr(F, f) });

	jit_insn_store_relative(F, ptr, 0, vec);
	jit_insn_store_relative(F, ptr, jit_type_get_offset(type, 1), jit_value_create_nint_constant(F, jit_type_int, 0));

	jit_type_free(type);
	return val;
}

static int32_t jit_set_insert(jit_set_* set, void* v) {
	return set->insert(v).second;
}

jit_value_t jit_set::insert_move_inc(jit_function_t F, const Type& value_type, jit_value_t s, jit_value_t v)
{
	jit_value_t set = jit_insn_load_relative(F, jit_insn_address_of(F, s), 0, LS_POINTER);

	jit_value_t space = jit_general::call_native(F, jit_type_void_ptr, { jit_type_uint }, (void*) jit_malloc, { jit_value_create_nint_constant(F, jit_type_uint, value_type.bytes()) });
	jit_value_t copy = jit_general::move_inc(F, v, value_type);
	jit_insn_store_relative(F, space, 0, copy);

	jit_value_t res = jit_general::call_native(F, LS_BOOLEAN, { LS_POINTER, LS_POINTER }, (void*) jit_set_insert, { set, space });

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if(F, res, &exit);

	jit_general::delete_ref(F, copy, value_type);
	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_free, { space });

	jit_insn_label(F, &exit);
	return res;
}

static jit_set_::iterator jit_set_begin(jit_set_* set) {
	return set->begin();
}
static jit_set_::iterator jit_set_end(jit_set_* set) {
	return set->end();
}

std::pair<jit_value_t, jit_value_t> jit_set::begin_end(jit_function_t F, jit_value_t s)
{
	jit_value_t set  = jit_insn_load_relative(F, jit_insn_address_of(F, s), 0, jit_type_void_ptr);
	jit_value_t begin = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_set_begin, { set });
	jit_value_t end   = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_set_end, { set });
	return std::make_pair(begin, end);
}

static jit_set_::iterator jit_set_inc(jit_set_::iterator it) {
	return ++it;
}

jit_value_t jit_set::inc_iterator(jit_function_t F, jit_value_t i)
{
	return jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_set_inc, { i });
}

static void* jit_set_load(jit_set_::iterator it) {
	return *it;
}

jit_value_t jit_set::load_iterator(jit_function_t F, const Type& value_type, jit_value_t i)
{
	jit_type_t jit_value_type = value_type.jit_type();
	jit_value_t ptr = jit_general::call_native(F, jit_type_void_ptr, { jit_type_void_ptr }, (void*) jit_set_load, { i });
	jit_value_t res = jit_insn_load_relative(F, ptr, 0, jit_value_type);
	jit_type_free(jit_value_type);
	return res;
}

static int32_t jit_set_size(jit_set_* set) {
	return set->size();
}

jit_value_t jit_set::size(jit_function_t F, jit_value_t s)
{
	jit_value_t set = jit_insn_load_relative(F, jit_insn_address_of(F, s), 0, LS_POINTER);
	return jit_general::call_native(F, LS_I32, { LS_POINTER }, (void*) jit_set_size, { set });
}

static void jit_set_delete(jit_set_* set) {
	delete set;
}

void jit_set::delete_temporary(jit_function_t F, const Type& value_type, jit_value_t s)
{
	jit_type_t jit_type = jit_set::jit_type();
	jit_value_t ptr = jit_insn_address_of(F, s);
	jit_value_t set = jit_insn_load_relative(F, ptr, 0, LS_POINTER);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, 1), LS_I32);

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if(F, refs, &exit);

	pair<jit_value_t, jit_value_t> begin_end = jit_set::begin_end(F, s);
	jit_value_t begin = begin_end.first;
	jit_value_t end = begin_end.second;

	jit_label_t loop = jit_label_undefined;
	jit_label_t stop = jit_label_undefined;
	jit_insn_label(F, &loop);
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_value_t x = jit_set::load_iterator(F, value_type, begin);
	jit_general::delete_ref(F, x, value_type);

	jit_insn_store(F, begin, jit_set::inc_iterator(F, begin));

	jit_insn_branch(F, &loop);
	jit_insn_label(F, &stop);

	jit_general::call_native(F, LS_VOID, { LS_POINTER }, (void*) jit_set_delete, { set });

	jit_insn_label(F, &exit);
	jit_type_free(jit_type);
}

void jit_set::delete_ref(jit_function_t F, const Type& value_type, jit_value_t s)
{
	jit_type_t jit_type = jit_set::jit_type();

	jit_value_t ptr = jit_insn_address_of(F, s);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, 1), jit_type_int);

	jit_label_t exit = jit_label_undefined;

	jit_insn_branch_if_not(F, refs, &exit);

	jit_insn_store_relative(F, ptr, jit_type_get_offset(jit_type, 1), jit_insn_sub(F, refs, jit_general::constant_i32(F, 1)));

	jit_set::delete_temporary(F, value_type, s);

	jit_insn_label(F, &exit);
	jit_type_free(jit_type);
}

jit_value_t jit_set::move(jit_function_t F, const Type& value_type, jit_value_t s)
{
	jit_type_t jit_type = jit_set::jit_type();
	jit_type_t jit_value_type = value_type.jit_type();

	jit_value_t ptr  = jit_insn_address_of(F, s);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, 1), jit_type_int);

	jit_value_t copy = jit_value_create(F, jit_type);
	jit_insn_store(F, copy, s);

	jit_label_t exit = jit_label_undefined;
	jit_insn_branch_if_not(F, refs, &exit);

	jit_insn_store(F, copy, jit_set::create(F, value_type));

	pair<jit_value_t, jit_value_t> begin_end = jit_set::begin_end(F, s);
	jit_value_t begin = begin_end.first;
	jit_value_t end = begin_end.second;

	jit_label_t stop = jit_label_undefined;
	jit_label_t loop = jit_label_undefined;
	jit_insn_label(F, &loop);
	jit_insn_branch_if(F, jit_insn_eq(F, begin, end), &stop);

	jit_set::insert_move_inc(F, value_type, copy, jit_insn_load_relative(F, begin, 0, jit_value_type));

	jit_insn_store(F, begin, jit_set::inc_iterator(F, begin));

	jit_insn_branch(F, &loop);
	jit_insn_label(F, &stop);

	jit_insn_label(F, &exit);
	jit_type_free(jit_type);
	jit_type_free(jit_value_type);
	return copy;
}

jit_value_t jit_set::move_inc(jit_function_t F, const Type& value_type, jit_value_t s)
{
	jit_value_t copy = move(F, value_type, s);
	inc_refs(F, copy);
	return copy;
}

void jit_set::inc_refs(jit_function_t F, jit_value_t s)
{
	jit_type_t jit_type = jit_set::jit_type();

	jit_value_t ptr = jit_insn_address_of(F, s);
	jit_value_t refs = jit_insn_load_relative(F, ptr, jit_type_get_offset(jit_type, 1), jit_type_int);

	jit_insn_store_relative(F, ptr, jit_type_get_offset(jit_type, 1), jit_insn_add(F, refs, jit_general::constant_i32(F, 1)));

	jit_type_free(jit_type);
}
