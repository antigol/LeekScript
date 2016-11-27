#include <algorithm>
#include "ArraySTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

LSArray<LSValue*>* array_chunk_1_ptr(LSArray<LSValue*>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_int(LSArray<int>* array) {
	return array->ls_chunk(1);
}

LSArray<LSValue*>* array_chunk_1_float(LSArray<double>* array) {
	return array->ls_chunk(1);
}

LSValue* array_sub(LSArray<LSValue*>* array, int begin, int end) {
	LSValue* r = array->range(begin, end);
	if (array->refs == 0) delete array;
	return r;
}

ArraySTD::ArraySTD() : Module("Array") {

	operator_("<", {
		{Type::ARRAY, Type::ARRAY, Type::BOOLEAN, (void*) &ArraySTD::lt, Method::NATIVE}
	});

	operator_("in", {
		{Type::PTR_ARRAY, Type::POINTER, Type::BOOLEAN, (void*) &LSArray<LSValue*>::in},
		{Type::REAL_ARRAY, Type::REAL, Type::BOOLEAN, (void*) &LSArray<double>::in},
		{Type::INT_ARRAY, Type::INTEGER, Type::BOOLEAN, (void*) &LSArray<int>::in}
	});
/*
	Type tilde_tilde_fun_type = Type::FUNCTION_P;
	tilde_tilde_fun_type.setArgumentType(0, Type::T);
	tilde_tilde_fun_type.setReturnType(Type::POINTER);
	operator_("~~", {
		{Type::T_ARRAY, tilde_tilde_fun_type, Type::PTR_ARRAY, (void*) &LSArray<LSValue*>::ls_map}
	});
*/
	/*
	 * Methods
	 */
	method("average", {
		{Type::PTR_ARRAY, Type::REAL, {}, (void*) &LSArray<LSValue*>::ls_average},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_average},
		{Type::INT_ARRAY, Type::REAL, {}, (void*) &LSArray<int>::ls_average}
	});

	method("max", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_max},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_max},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_max}
	});

	method("min", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_min},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_min},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_min}
	});

	method("size", {
		{Type::ARRAY, Type::INTEGER, {}, (void*) &ArraySTD::size, Method::NATIVE}
	});

	method("sum", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_sum},
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_sum},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_sum}
	});

	Type map_int_fun_type = Type::FUNCTION_P;
	map_int_fun_type.setArgumentType(0, Type::INTEGER);
	map_int_fun_type.setReturnType(Type::POINTER);

	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::POINTER);
	map_fun_type.setReturnType(Type::POINTER);

	method("map", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::INT_ARRAY, Type::PTR_ARRAY, {map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	method("chunk", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY_ARRAY, {}, (void*) array_chunk_1_ptr},
		{Type::REAL_ARRAY, Type::REAL_ARRAY_ARRAY, {}, (void*) array_chunk_1_float},
		{Type::INT_ARRAY, Type::INT_ARRAY_ARRAY, {}, (void*) array_chunk_1_int},
		{Type::PTR_ARRAY, Type::PTR_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_chunk},
		{Type::REAL_ARRAY, Type::REAL_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<double>::ls_chunk},
		{Type::INT_ARRAY, Type::INT_ARRAY_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_chunk},
    });

	method("unique", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_unique},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_unique},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_unique},
	});

	method("sort", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_sort},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_sort},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_sort},
	});

	Type map2_fun_type = Type::FUNCTION_P;
	map2_fun_type.setArgumentType(0, Type::POINTER);
	map2_fun_type.setArgumentType(1, Type::POINTER);
	map2_fun_type.setReturnType(Type::POINTER);

	Type map2_fun_type_int = Type::FUNCTION_P;
	map2_fun_type_int.setArgumentType(0, Type::POINTER);
	map2_fun_type_int.setArgumentType(1, Type::INTEGER);
	map2_fun_type_int.setReturnType(Type::POINTER);

	method("map2", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::PTR_ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::ls_map2},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::ls_map2_int},
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
		{Type::PTR_ARRAY, Type::VOID, {iter_fun_type}, (void*) &LSArray<LSValue*>::ls_iter},
		{Type::REAL_ARRAY, Type::VOID, {iter_fun_type_float}, (void*) &LSArray<double>::ls_iter},
		{Type::INT_ARRAY, Type::VOID, {iter_fun_type_int}, (void*) &LSArray<int>::ls_iter},
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
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_filter},
		{Type::INT_ARRAY, Type::INT_ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_filter}
	});

	method("contains", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains},
		{Type::REAL_ARRAY, Type::BOOLEAN, {Type::REAL}, (void*) &LSArray<double>::ls_contains},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSArray<int>::ls_contains}
	});

	method("isEmpty", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<LSValue*>::ls_empty},
		{Type::REAL_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<double>::ls_empty},
		{Type::INT_ARRAY, Type::BOOLEAN, {}, (void*) &LSArray<int>::ls_empty},
	});

	method("isPermutation", {
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INT_ARRAY}, (void*) &LSArray<int>::is_permutation}
	});

	method("partition", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition},
		{Type::REAL_ARRAY, Type::PTR_ARRAY, {pred_fun_type_float}, (void*) &LSArray<double>::ls_partition},
		{Type::INT_ARRAY, Type::PTR_ARRAY, {pred_fun_type_int}, (void*) &LSArray<int>::ls_partition}
	});

	method("first", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::REAL_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_first},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_first},
	});
	method("last", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::REAL_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_last},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_last},
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

	method("foldLeft", {
		{Type::PTR_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldLeft},
		{Type::REAL_ARRAY, Type::POINTER, {fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldLeft},
		{Type::INT_ARRAY, Type::POINTER, {fold_fun_type_int, Type::POINTER}, (void*) &LSArray<int>::ls_foldLeft},
	});
	method("foldRight", {
		{Type::PTR_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldRight},
		{Type::REAL_ARRAY, Type::POINTER, {fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldRight},
		{Type::INT_ARRAY, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSArray<int>::ls_foldRight},
	});

	method("search", {
		{Type::PTR_ARRAY, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search},
		{Type::REAL_ARRAY, Type::INTEGER, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_search},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search}
	});

	method("pop", {
		{Type::PTR_ARRAY, Type::POINTER, {}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::REAL_ARRAY, Type::POINTER, {}, (void*) &LSArray<double>::ls_pop},
		{Type::INT_ARRAY, Type::POINTER, {}, (void*) &LSArray<int>::ls_pop}
	});

	method("product", {
		{Type::REAL_ARRAY, Type::REAL, {}, (void*) &LSArray<double>::ls_product},
		{Type::INT_ARRAY, Type::INTEGER, {}, (void*) &LSArray<int>::ls_product}
	});

	method("push", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL}, (void*) &LSArray<double>::ls_push},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER}, (void*) &LSArray<int>::ls_push},
	});

	method("pushAll", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_ptr},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_flo},
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<LSValue*>::ls_push_all_int},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<double>::ls_push_all_ptr},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_push_all_flo},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<double>::ls_push_all_int},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<int>::ls_push_all_ptr},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<int>::ls_push_all_flo},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_push_all_int},
	});

	method("join", {
		{Type::PTR_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<LSValue*>::ls_join},
		{Type::REAL_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<double>::ls_join},
		{Type::INT_ARRAY, Type::STRING, {Type::STRING}, (void*) &LSArray<int>::ls_join}
	});

	method("json", {
		{Type::ARRAY, Type::STRING, {}, (void*) &LSValue::ls_json},
	});

	method("clear", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_clear},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_clear},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_clear},
	});

	method("fill", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_fill},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill}
	});

	method("insert", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_insert},
		{Type::INT_ARRAY, Type::INT_ARRAY, {Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_insert}
	});

	method("remove", {
		{Type::PTR_ARRAY, Type::POINTER, {Type::INTEGER}, (void*)&LSArray<LSValue*>::ls_remove},
		{Type::REAL_ARRAY, Type::REAL, {Type::INTEGER}, (void*)&LSArray<double>::ls_remove},
		{Type::INT_ARRAY, Type::INTEGER, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove},
	});

	method("removeElement", {
		{Type::PTR_ARRAY, Type::BOOLEAN, {Type::POINTER}, (void*)&LSArray<LSValue*>::ls_remove_element},
		{Type::REAL_ARRAY, Type::BOOLEAN, {Type::REAL}, (void*)&LSArray<double>::ls_remove_element},
		{Type::INT_ARRAY, Type::BOOLEAN, {Type::INTEGER}, (void*)&LSArray<int>::ls_remove_element},
	});

	method("reverse", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_reverse},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_reverse},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_reverse},
	});

	method("shuffle", {
		{Type::PTR_ARRAY, Type::PTR_ARRAY, {}, (void*) &LSArray<LSValue*>::ls_shuffle},
		{Type::REAL_ARRAY, Type::REAL_ARRAY, {}, (void*) &LSArray<double>::ls_shuffle},
		{Type::INT_ARRAY, Type::INT_ARRAY, {}, (void*) &LSArray<int>::ls_shuffle},
	});


	/*
	 * Static methods
	 */
	static_method("average", {
		{Type::REAL, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_average},
		{Type::REAL, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_average},
		{Type::REAL, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_average}
	});

	static_method("size", {
		{Type::INTEGER, {Type::POINTER}, (void*) &LSArray<LSValue*>::ls_size},
		{Type::INTEGER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_size},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_size}
	});

	static_method("sum", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_sum},
		{Type::POINTER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_sum},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_sum}
	});

	static_method("map", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, map_fun_type}, (void*) &LSArray<LSValue*>::ls_map},
		{Type::PTR_ARRAY, {Type::INT_ARRAY, map_int_fun_type}, (void*) &LSArray<int>::ls_map},
	});

	static_method("map2", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::PTR_ARRAY, map2_fun_type}, (void*) &LSArray<LSValue*>::ls_map2},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INT_ARRAY, map2_fun_type_int}, (void*) &LSArray<LSValue*>::ls_map2_int},
	});

	static_method("iter", {
		{Type::VOID, {Type::PTR_ARRAY, iter_fun_type},(void*) &LSArray<LSValue*>::ls_iter},
		{Type::VOID, {Type::REAL_ARRAY, iter_fun_type_float},(void*) &LSArray<double>::ls_iter},
		{Type::VOID, {Type::INT_ARRAY, iter_fun_type_int},(void*) &LSArray<int>::ls_iter},
	});

	static_method("filter", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_filter},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_filter},
		{Type::INT_ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_filter},
	});

	static_method("contains", {
		{Type::BOOLEAN, {Type::PTR_ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_contains},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_contains},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_contains}
	});

	static_method("isEmpty", {
		{Type::BOOLEAN, {Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_empty},
		{Type::BOOLEAN, {Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_empty},
		{Type::BOOLEAN, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_empty}
	});

	static_method("partition", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, pred_fun_type}, (void*) &LSArray<LSValue*>::ls_partition},
		{Type::PTR_ARRAY, {Type::REAL_ARRAY, pred_fun_type_float}, (void*) &LSArray<double>::ls_partition},
		{Type::PTR_ARRAY, {Type::INT_ARRAY, pred_fun_type_int}, (void*) &LSArray<int>::ls_partition}
	});

	static_method("first", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_first},
		{Type::POINTER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_first},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_first},
	});

	static_method("last", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_last},
		{Type::POINTER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_last},
		{Type::POINTER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_last},
	});

	static_method("foldLeft", {
		{Type::POINTER, {Type::PTR_ARRAY, fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldLeft},
		{Type::POINTER, {Type::REAL_ARRAY, fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldLeft},
		{Type::POINTER, {Type::INT_ARRAY, fold_fun_type_int, Type::POINTER}, (void*) &LSArray<int>::ls_foldLeft}
	});

	static_method("foldRight", {
		{Type::POINTER, {Type::PTR_ARRAY, fold_fun_type, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_foldRight},
		{Type::POINTER, {Type::REAL_ARRAY, fold_fun_type_float, Type::POINTER}, (void*) &LSArray<double>::ls_foldRight},
		{Type::POINTER, {Type::INT_ARRAY, fold_fun_type_int, Type::POINTER}, (void*) &LSArray<int>::ls_foldRight}
	});

	static_method("shuffle", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_shuffle},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_shuffle},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_shuffle},
	});
	static_method("reverse", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_reverse},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_reverse},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_reverse},
	});

	static_method("search", {
		{Type::INTEGER, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_search},
		{Type::INTEGER, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_search},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_search}
	});

	static_method("subArray", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INTEGER, Type::INTEGER}, (void* )&array_sub},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INTEGER, Type::INTEGER}, (void* )&array_sub},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void* )&array_sub},
	});

	static_method("pop", {
		{Type::POINTER, {Type::PTR_ARRAY}, (void*) &LSArray<LSValue*>::ls_pop},
		{Type::INTEGER, {Type::REAL_ARRAY}, (void*) &LSArray<double>::ls_pop},
		{Type::INTEGER, {Type::INT_ARRAY}, (void*) &LSArray<int>::ls_pop}
	});

	static_method("push", {
		{Type::PTR_ARRAY, {Type::POINTER, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_push},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_push},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_push}
	});

	static_method("pushAll", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_ptr},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_flo},
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<LSValue*>::ls_push_all_int},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<double>::ls_push_all_ptr},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_push_all_flo},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<double>::ls_push_all_int},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::PTR_ARRAY}, (void*)&LSArray<int>::ls_push_all_ptr},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::REAL_ARRAY}, (void*)&LSArray<int>::ls_push_all_flo},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INT_ARRAY}, (void*)&LSArray<int>::ls_push_all_int},
	});

	static_method("join", {
		{Type::STRING, {Type::PTR_ARRAY, Type::STRING}, (void*) &LSArray<LSValue*>::ls_join},
		{Type::STRING, {Type::REAL_ARRAY, Type::STRING}, (void*) &LSArray<double>::ls_join},
		{Type::STRING, {Type::INT_ARRAY, Type::STRING}, (void*) &LSArray<int>::ls_join}
	});

	static_method("clear", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY}, (void*)&LSArray<LSValue*>::ls_clear},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY}, (void*)&LSArray<double>::ls_clear},
		{Type::INT_ARRAY, {Type::INT_ARRAY}, (void*)&LSArray<int>::ls_clear},
	});

	static_method("fill", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_fill},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<double>::ls_fill},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_fill}
	});

	static_method("insert", {
		{Type::PTR_ARRAY, {Type::PTR_ARRAY, Type::POINTER, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert},
		{Type::REAL_ARRAY, {Type::REAL_ARRAY, Type::REAL, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_insert},
		{Type::INT_ARRAY, {Type::INT_ARRAY, Type::INTEGER, Type::INTEGER}, (void*) &LSArray<int>::ls_insert}
	});

	static_method("remove", {
		{Type::POINTER, {Type::PTR_ARRAY, Type::INTEGER}, (void*) &LSArray<LSValue*>::ls_remove},
		{Type::REAL, {Type::REAL_ARRAY, Type::INTEGER}, (void*) &LSArray<double>::ls_remove},
		{Type::INTEGER, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove}
	});

	static_method("removeElement", {
		{Type::BOOLEAN, {Type::PTR_ARRAY, Type::POINTER}, (void*) &LSArray<LSValue*>::ls_remove_element},
		{Type::BOOLEAN, {Type::REAL_ARRAY, Type::REAL}, (void*) &LSArray<double>::ls_remove_element},
		{Type::BOOLEAN, {Type::INT_ARRAY, Type::INTEGER}, (void*) &LSArray<int>::ls_remove_element}
	});
}

Compiler::value ArraySTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
		return b->rlt(a);
	});
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return res;
}

Compiler::value ArraySTD::size(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_array_size(args[0]);
	c.insn_delete(args[0]);
	return res;
}

}
