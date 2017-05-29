#include "MapSTD.hpp"
#include "../value/LSMap.hpp"

using namespace std;

namespace ls {

int map_size(const LSMap<LSValue*,LSValue*>* map) {
	int r = map->size();
	if (map->refs == 0) delete map;
	return r;
}

MapSTD::MapSTD() : Module("Map") {

	LSMap<LSValue*, LSValue*>::clazz = clazz;
	LSMap<LSValue*, int>::clazz = clazz;
	LSMap<LSValue*, double>::clazz = clazz;
	LSMap<int, LSValue*>::clazz = clazz;
	LSMap<int, int>::clazz = clazz;
	LSMap<int, double>::clazz = clazz;
	LSMap<double, LSValue*>::clazz = clazz;
	LSMap<double, int>::clazz = clazz;
	LSMap<double, double>::clazz = clazz;

	operator_("in", {
		{Type::PTR_PTR_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, LSValue*>::in, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, double>::in, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, int>::in, Method::NATIVE},
		{Type::REAL_PTR_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, LSValue*>::in, Method::NATIVE},
		{Type::REAL_REAL_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, double>::in, Method::NATIVE},
		{Type::REAL_INT_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, int>::in, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, LSValue*>::in, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, double>::in, Method::NATIVE},
		{Type::INT_INT_MAP, Type::LONG, Type::BOOLEAN, (void*) &LSMap<int, int>::in, Method::NATIVE},
	});

	method("size", {
		{Type::PTR_PTR_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INTEGER, {}, (void*) map_size, Method::NATIVE},
    });

	method("values", {
		{Type::PTR_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<LSValue*, LSValue*>::values, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<LSValue*, double>::values, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<LSValue*, int>::values, Method::NATIVE},
		{Type::REAL_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<double, LSValue*>::values, Method::NATIVE},
		{Type::REAL_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<double, double>::values, Method::NATIVE},
		{Type::REAL_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<double, int>::values, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<int, LSValue*>::values, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<int, double>::values, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<int, int>::values, Method::NATIVE}
	});

	method("insert", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_insert, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::BOOLEAN, {Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_insert, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_insert, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_insert, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::BOOLEAN, {Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_insert, Method::NATIVE},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_insert, Method::NATIVE},
    });

	method("clear", {
		{Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::PTR_REAL_MAP, {}, (void*) &LSMap<LSValue*,double>::ls_clear, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::PTR_INT_MAP, {}, (void*) &LSMap<LSValue*,int>::ls_clear, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::INT_PTR_MAP, {}, (void*) &LSMap<int,LSValue*>::ls_clear, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::INT_REAL_MAP, {}, (void*) &LSMap<int,double>::ls_clear, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INT_INT_MAP, {}, (void*) &LSMap<int,int>::ls_clear, Method::NATIVE},
	});

	method("erase", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,double>::ls_erase, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,int>::ls_erase, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,LSValue*>::ls_erase, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,double>::ls_erase, Method::NATIVE},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,int>::ls_erase, Method::NATIVE},
	});

	method("look", {
		{Type::PTR_PTR_MAP, Type::POINTER, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_look, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::REAL, {Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_look, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_look, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::POINTER, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_look, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_look, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_look, Method::NATIVE},
	});
}

}
