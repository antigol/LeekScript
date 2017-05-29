#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

namespace ls {

ArraySTD::ArraySTD() : Module("Array") {

	LSArray<LSValue*>::clazz = clazz;
	LSArray<int>::clazz = clazz;
	LSArray<double>::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("<", {
		{Type::ARRAY, Type::ARRAY, Type::BOOLEAN, (void*) &ArraySTD::lt}
	});
	operator_("in", {
		{Type::PTR_ARRAY, Type::POINTER, Type::BOOLEAN, (void*) &LSArray<LSValue*>::in, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, Type::BOOLEAN, (void*) &LSArray<double>::in, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, Type::BOOLEAN, (void*) &LSArray<int>::in_i, Method::NATIVE}
	});
	operator_("+=", {
		{Type::REAL_ARRAY, Type::REAL, Type::REAL, (void*) &LSArray<double>::push_move, Method::NATIVE}
	});
	/* Type tilde_tilde_fun_type = Type::FUNCTION_P;
	tilde_tilde_fun_type.setArgumentType(0, Type::T);
	tilde_tilde_fun_type.setReturnType(Type::POINTER);
	operator_("~~", {
		{Type::T_ARRAY, tilde_tilde_fun_type, Type::PTR_ARRAY, (void*) &LSArray<LSValue*>::ls_map}
	});*/
	/*
	 * Methods
	 */
	method("average", {
		{Type::PTR_ARRAY, Type::REAL, {}, (void*) &LSArray<LSValue*>::ls_average, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_average, Method::NATIVE},
		{Type::INT_ARRAY, Type::REAL, {}, (void*) &LSArray<int>::ls_average, Method::NATIVE}
	});

	method("chunk", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY_ARRAY, {}, (void*) ArraySTD::chunk_1_ptr, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY_ARRAY, {}, (void*) ArraySTD::chunk_1_float, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY_ARRAY, {}, (void*) ArraySTD::chunk_1_int, Method::NATIVE},
		{Type::PTR_ARRAY, Type::PTR_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_chunk, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<double>::ls_chunk, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_chunk, Method::NATIVE},
    });

	method("clear", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_clear, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_clear, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_clear, Method::NATIVE},
	});

	method("contains", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::REAL_ARRAY, Type::BOOLEAN, {Type::REAL}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSArray<int>::ls_contains, Method::NATIVE}
	});

	Type iter_fun_type = Type::FUNCTION_P;
	iter_fun_type.setArgumentType(0, Type::POINTER);
	iter_fun_type.setReturnType(Type::VOID);
	Type iter_fun_type_int = Type::FUNCTION_P;
	iter_fun_type_int.setArgumentType(0, Type::INTEGER);
	iter_fun_type_int.setReturnType(Type::VOID);
	Type iter_fun_type_float = Type::FUNCTION_P;
	iter_fun_type_float.setArgumentType(0, Type::REAL);
	iter_fun_type_float.setReturnType(Type::VOID);

	method("iter", {
		{Type::PTR_ARRAY, Type::VOID, {iter_fun_type}, (void*) &LSArray<LSValue*>::ls_iter, Method::NATIVE},
		{Type::REAL_ARRAY, Type::VOID, {iter_fun_type_float}, (void*) &LSArray<double>::ls_iter, Method::NATIVE},
		{Type::INT_ARRAY, Type::VOID, {iter_fun_type_int}, (void*) &LSArray<int>::ls_iter, Method::NATIVE},
	});

	method("max", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_max, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_max, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_max, Method::NATIVE}
	});

	method("min", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_min, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_min, Method::NATIVE}
	});

	Type map_int_fun_type = Type::FUNCTION_P;
	map_int_fun_type.setArgumentType(0, Type::INTEGER);
	map_int_fun_type.setReturnType(Type::INTEGER);

	Type map_int_ptr_fun_type = Type::FUNCTION_P;
	map_int_ptr_fun_type.setArgumentType(0, Type::INTEGER);
	map_int_ptr_fun_type.setReturnType(Type::POINTER);

	Type map_real_fun_type = Type::FUNCTION_P;
	map_real_fun_type.setArgumentType(0, Type::REAL);
	map_real_fun_type.setReturnType(Type::REAL);

	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);

	auto map_ptr = &LSArray<LSValue*>::ls_map<LSValue*>;
	auto map_real = &LSArray<double>::ls_map<double>;
	auto map_int = &LSArray<int>::ls_map<int>;
	auto map_int_ptr = &LSArray<int>::ls_map<LSValue*>;

	method("map", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {map_fun_type}, (void*) map_ptr, Method::NATIVE},
		{Type::INT_ARRAY, Type::PTR_ARRAY, {map_int_ptr_fun_type}, (void*) map_int_ptr, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {map_int_fun_type}, (void*) map_int, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {map_real_fun_type}, (void*) map_real, Method::NATIVE}
	});

	method("unique", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_unique, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_unique, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_unique, Method::NATIVE},
	});

	method("sort", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_sort, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_sort, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_sort, Method::NATIVE},
	});

	Type map2_fun_type = Type::FUNCTION_P;
	map2_fun_type.setArgumentType(0, Type::POINTER);
	map2_fun_type.setArgumentType(1, Type::POINTER);
	map2_fun_type.setReturnType(Type::POINTER);

	Type map2_fun_type_int = Type::FUNCTION_P;
	map2_fun_type_int.setArgumentType(0, Type::POINTER);
	map2_fun_type_int.setArgumentType(1, Type::INTEGER);
	map2_fun_type_int.setReturnType(Type::POINTER);

	auto map2_ptr_ptr = &LSArray<LSValue*>::ls_map2<LSValue*, LSValue*>;
	auto map2_ptr_int = &LSArray<LSValue*>::ls_map2<LSValue*, int>;

	method("map2", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::PTR_ARRAY, map2_fun_type}, (void*) map2_ptr_ptr, Method::NATIVE},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::INT_ARRAY, map2_fun_type_int}, (void*) map2_ptr_int, Method::NATIVE},
	});

	Type pred_fun_type = Type::FUNCTION_P;
	pred_fun_type.setArgumentType(0, Type::POINTER);
	pred_fun_type.setReturnType(Type::BOOLEAN);
	Type pred_fun_type_float = Type::FUNCTION_P;
	pred_fun_type_float.setArgumentType(0, Type::REAL);
	pred_fun_type_float.setReturnType(Type::BOOLEAN);
	Type pred_fun_type_int = Type::FUNCTION_P;
	pred_fun_type_int.setArgumentType(0, Type::INTEGER);
	pred_fun_type_int.setReturnType(Type::BOOLEAN);

	method("filter", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_filter, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_filter, Method::NATIVE}
	});

	method("isEmpty", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<LSValue*>::ls_empty, Method::NATIVE},
		{Type::REAL_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<double>::ls_empty, Method::NATIVE},
		{Type::INT_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<int>::ls_empty, Method::NATIVE},
	});

	method("isPermutation", {
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INT_ARRAY}, (void*) &LSArray<int>::is_permutation, Method::NATIVE}
	});

	method("partition", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition, Method::NATIVE},
		{Type::REAL_ARRAY, Type::PTR_ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_partition, Method::NATIVE},
		{Type::INT_ARRAY, Type::PTR_ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_partition, Method::NATIVE}
	});

	method("first", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_first, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_first, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_first, Method::NATIVE},
	});

	method("last", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_last, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_last, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_last, Method::NATIVE},
	});

	Type fold_fun_type = Type::FUNCTION_P;
	fold_fun_type.setArgumentType(0, Type::POINTER);
	fold_fun_type.setArgumentType(1, Type::POINTER);
	fold_fun_type.setReturnType(Type::POINTER);
	Type fold_fun_type_float = Type::FUNCTION_P;
	fold_fun_type_float.setArgumentType(0, Type::POINTER);
	fold_fun_type_float.setArgumentType(1, Type::REAL);
	fold_fun_type_float.setReturnType(Type::POINTER);
	Type fold_fun_type_int = Type::FUNCTION_P;
	fold_fun_type_int.setArgumentType(0, Type::POINTER);
	fold_fun_type_int.setArgumentType(1, Type::INTEGER);
	fold_fun_type_int.setReturnType(Type::POINTER);

	auto fold_left_ptr = &LSArray<LSValue*>::ls_foldLeft<LSValue*>;
	auto fold_left_int = &LSArray<int>::ls_foldLeft<int>;
	auto fold_left_real = &LSArray<double>::ls_foldLeft<double>;

	method("foldLeft", {
		{Type::PTR_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) fold_left_ptr, Method::NATIVE},
		{Type::REAL_ARRAY, Type::POINTER, {fold_fun_type_float, Type::POINTER}, (void*) fold_left_real, Method::NATIVE},
		{Type::INT_ARRAY, Type::POINTER, {fold_fun_type_int, Type::POINTER}, (void*) fold_left_int, Method::NATIVE},
	});

	Type fold_right_fun_type = Type::FUNCTION_P;
	fold_right_fun_type.setArgumentType(0, Type::POINTER);
	fold_right_fun_type.setArgumentType(1, Type::POINTER);
	fold_right_fun_type.setReturnType(Type::POINTER);
	Type fold_right_fun_type_float = Type::FUNCTION_P;
	fold_right_fun_type_float.setArgumentType(0, Type::REAL);
	fold_right_fun_type_float.setArgumentType(1, Type::POINTER);
	fold_right_fun_type_float.setReturnType(Type::POINTER);
	Type fold_right_fun_type_int = Type::FUNCTION_P;
	fold_right_fun_type_int.setArgumentType(0, Type::INTEGER);
	fold_right_fun_type_int.setArgumentType(1, Type::POINTER);
	fold_right_fun_type_int.setReturnType(Type::POINTER);

	auto fold_right_ptr = &LSArray<LSValue*>::ls_foldRight<LSValue*>;
	auto fold_right_int = &LSArray<int>::ls_foldRight<int>;
	auto fold_right_real = &LSArray<double>::ls_foldRight<double>;

	method("foldRight", {
		{Type::PTR_ARRAY, Type::POINTER, {fold_right_fun_type, Type::POINTER}, (void*) &fold_right_ptr, Method::NATIVE},
		{Type::REAL_ARRAY, Type::POINTER, {fold_right_fun_type_float, Type::POINTER}, (void*) &fold_right_real, Method::NATIVE},
		{Type::INT_ARRAY, Type::POINTER, {fold_right_fun_type, Type::POINTER}, (void*) &fold_right_int, Method::NATIVE},
	});

	method("pop", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_pop, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_pop, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_pop, Method::NATIVE}
	});

	method("product", {
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_product, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_product, Method::NATIVE}
	});

	method("push", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL}, (void*) &LSArray<double>::ls_push, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_push, Method::NATIVE},
	});

	method("pushAll", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_flo, Method::NATIVE},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_int, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<double>::ls_push_all_ptr, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_push_all_flo, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<double>::ls_push_all_int, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<int>::ls_push_all_ptr, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<int>::ls_push_all_flo, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_push_all_int, Method::NATIVE},
	});

	method("join", {
		{Type::PTR_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<LSValue*>::ls_join, Method::NATIVE},
		{Type::REAL_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<double>::ls_join, Method::NATIVE},
		{Type::INT_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<int>::ls_join, Method::NATIVE}
	});

	method("json", {
		{Type::ARRAY, Type::STRING, {}, (void*) &LSValue::ls_json, Method::NATIVE},
	});

	method("fill", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_fill, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill, Method::NATIVE}
	});

	method("insert", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_insert, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_insert, Method::NATIVE}
	});

	method("random", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_random, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::INTEGER}, (void*) &LSArray<double>::ls_random, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_random, Method::NATIVE}
	});

	method("remove", {
		{Type::PTR_ARRAY, Type::POINTER, {Type::INTEGER}, (void*)&LSArray<LSValue*>::ls_remove, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {Type::INTEGER}, (void*)&LSArray<double>::ls_remove, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove, Method::NATIVE},
	});

	method("removeElement", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*)&LSArray<LSValue*>::ls_remove_element, Method::NATIVE},
		{Type::REAL_ARRAY, Type::BOOLEAN, {Type::REAL}, (void*)&LSArray<double>::ls_remove_element, Method::NATIVE},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove_element, Method::NATIVE},
	});

	method("reverse", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_reverse, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_reverse, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_reverse, Method::NATIVE},
	});

	method("shuffle", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_shuffle, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_shuffle, Method::NATIVE},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_shuffle, Method::NATIVE},
	});

	method("search", {
		{Type::PTR_ARRAY, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search, Method::NATIVE},
		{Type::REAL_ARRAY, Type::INTEGER, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_search, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search, Method::NATIVE}
	});

	method("size", {
		{Type::ARRAY, Type::INTEGER, {}, (void*) &ArraySTD::size}
	});

	method("sum", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_sum, Method::NATIVE},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_sum, Method::NATIVE},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_sum, Method::NATIVE}
	});

	/*
	 * Static methods
	 */
	static_method("average", {
		{Type::REAL, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_average, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_average, Method::NATIVE},
		{Type::REAL, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_average, Method::NATIVE}
	});

	static_method("clear", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_clear, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_clear, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_clear, Method::NATIVE},
	});

	static_method("contains", {
		{Type::BOOLEAN, {Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_contains, Method::NATIVE}
	});

	static_method("filter", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_filter, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_filter, Method::NATIVE},
	});

	static_method("isEmpty", {
		{Type::BOOLEAN, {Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_empty, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_empty, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_empty, Method::NATIVE}
	});

	static_method("iter", {
		{Type::VOID, {Type::PTR_ARRAY, iter_fun_type},(void*) &LSArray<LSValue*>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::REAL_ARRAY, iter_fun_type_float},(void*) &LSArray<double>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::INT_ARRAY, iter_fun_type_int},(void*) &LSArray<int>::ls_iter, Method::NATIVE},
	});

	static_method("map", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, map_fun_type}, (void*) map_ptr, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, map_int_fun_type}, (void*) map_int, Method::NATIVE}
	});

	static_method("map2", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::PTR_ARRAY, map2_fun_type}, (void*) map2_ptr_ptr, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INT_ARRAY, map2_fun_type_int}, (void*) map2_ptr_int, Method::NATIVE},
	});

	static_method("partition", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::REAL_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_partition, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_partition, Method::NATIVE}
	});

	static_method("first", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_first, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_first, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_first, Method::NATIVE},
	});

	static_method("last", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_last, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_last, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_last, Method::NATIVE},
	});

	static_method("foldLeft", {
		{Type::POINTER, {Type::PTR_ARRAY, fold_fun_type, Type::POINTER}, (void*) fold_left_ptr, Method::NATIVE},
		{Type::POINTER, {Type::REAL_ARRAY, fold_fun_type_float, Type::POINTER}, (void*) fold_left_real, Method::NATIVE},
		{Type::POINTER, {Type::INT_ARRAY, fold_fun_type_int, Type::POINTER}, (void*) fold_left_int, Method::NATIVE}
	});

	static_method("foldRight", {
		{Type::POINTER, {Type::PTR_ARRAY, fold_right_fun_type, Type::POINTER}, (void*) fold_right_ptr, Method::NATIVE},
		{Type::POINTER, {Type::REAL_ARRAY, fold_right_fun_type_float, Type::POINTER}, (void*) fold_right_real, Method::NATIVE},
		{Type::POINTER, {Type::INT_ARRAY, fold_right_fun_type_int, Type::POINTER}, (void*) fold_right_int, Method::NATIVE}
	});

	static_method("shuffle", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_shuffle, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_shuffle, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_shuffle, Method::NATIVE},
	});

	static_method("random", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_random, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INTEGER}, (void*) &LSArray<double>::ls_random, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_random, Method::NATIVE}
	});

	static_method("reverse", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_reverse, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_reverse, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_reverse, Method::NATIVE},
	});

	static_method("search", {
		{Type::INTEGER, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search, Method::NATIVE},
		{Type::INTEGER, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_search, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search, Method::NATIVE}
	});

	static_method("subArray", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INTEGER, Type::INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INTEGER, Type::INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void* ) &ArraySTD::sub, Method::NATIVE},
	});

	static_method("pop", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_pop, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_pop, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_pop, Method::NATIVE}
	});

	static_method("push", {
		{Type::PTR_ARRAY, {Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_push, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_push, Method::NATIVE}
	});

	static_method("pushAll", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_ptr, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_flo, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_int, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<double>::ls_push_all_ptr, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_push_all_flo, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<double>::ls_push_all_int, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<int>::ls_push_all_ptr, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<int>::ls_push_all_flo, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<int>::ls_push_all_int, Method::NATIVE},
	});

	static_method("join", {
		{Type::STRING, {Type::PTR_ARRAY, Type::STRING}, (void*) &LSArray<LSValue*>::ls_join, Method::NATIVE},
		{Type::STRING, {Type::REAL_ARRAY, Type::STRING}, (void*) &LSArray<double>::ls_join, Method::NATIVE},
		{Type::STRING, {Type::INT_ARRAY, Type::STRING}, (void*) &LSArray<int>::ls_join, Method::NATIVE}
	});

	static_method("fill", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_fill, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill, Method::NATIVE}
	});

	static_method("insert", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_insert, Method::NATIVE}
	});

	static_method("remove", {
		{Type::POINTER, {Type::PTR_ARRAY, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_remove, Method::NATIVE},
		{Type::REAL, {Type::REAL_ARRAY, Type::INTEGER}, (void*) &LSArray<double>::ls_remove, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove, Method::NATIVE}
	});

	static_method("removeElement", {
		{Type::BOOLEAN, {Type::PTR_ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_remove_element, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_remove_element, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove_element, Method::NATIVE}
	});

	static_method("size", {
		{Type::INTEGER, {Type::POINTER}, (void*) &ArraySTD::size},
		{Type::INTEGER, {Type::REAL_ARRAY}, (void*) &ArraySTD::size},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &ArraySTD::size}
	});

	static_method("sort", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_sort, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_sort, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_sort, Method::NATIVE},
	});

	static_method("sum", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_sum, Method::NATIVE},
		{Type::POINTER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_sum, Method::NATIVE},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_sum, Method::NATIVE}
	});
}

Compiler::value ArraySTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
		return a->lt(b);
	});
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ArraySTD::size(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_array_size(args[0]);
	c.insn_delete_temporary(args[0]);
	return res;
}

LSArray<LSValue*>* ArraySTD::chunk_1_ptr(LSArray<LSValue*>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* ArraySTD::chunk_1_int(LSArray<int>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* ArraySTD::chunk_1_float(LSArray<double>* array) {
	return array->ls_chunk(1);
}

LSValue* ArraySTD::sub(LSArray<LSValue*>* array, int begin, int end) {
	LSValue* r = array->range(begin, end);
	if (array->refs == 0) delete array;
	return r;
}

}
