#include "MapSTD.hpp"
#include "../value/LSMap.hpp"


using namespace std;

namespace ls {
/*
int map_size(const LSMap<LSValue*,LSValue*>* map) {
	int r = map->size();
	if (map->refs == 0) delete map;
	return r;
}
*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
MapSTD::MapSTD() : Module("Map") {
/*
	method("size", {
		{Type::PTR_PTR_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::PTR_FLOAT_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::PTR_INT_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_PTR_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_FLOAT_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_INT_MAP, Type::INTEGER, {}, (void*) map_size},
    });

	method("insert", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_insert},
		{Type::PTR_FLOAT_MAP, Type::BOOLEAN, {Type::POINTER, Type::FLOAT}, (void*) &LSMap<LSValue*,double>::ls_insert},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_insert},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_insert},
		{Type::INT_FLOAT_MAP, Type::BOOLEAN, {Type::INTEGER, Type::FLOAT}, (void*) &LSMap<int,double>::ls_insert},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_insert},
    });

	method("clear", {
		{Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear},
		{Type::PTR_FLOAT_MAP, Type::PTR_FLOAT_MAP, {}, (void*) &LSMap<LSValue*,double>::ls_clear},
		{Type::PTR_INT_MAP, Type::PTR_INT_MAP, {}, (void*) &LSMap<LSValue*,int>::ls_clear},
		{Type::INT_PTR_MAP, Type::INT_PTR_MAP, {}, (void*) &LSMap<int,LSValue*>::ls_clear},
		{Type::INT_FLOAT_MAP, Type::INT_FLOAT_MAP, {}, (void*) &LSMap<int,double>::ls_clear},
		{Type::INT_INT_MAP, Type::INT_INT_MAP, {}, (void*) &LSMap<int,int>::ls_clear},
	});

	method("erase", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase},
		{Type::PTR_FLOAT_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,double>::ls_erase},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,int>::ls_erase},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,LSValue*>::ls_erase},
		{Type::INT_FLOAT_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,double>::ls_erase},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,int>::ls_erase},
	});

	method("look", {
		{Type::PTR_PTR_MAP, Type::POINTER, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_look},
		{Type::PTR_FLOAT_MAP, Type::FLOAT, {Type::POINTER, Type::FLOAT}, (void*) &LSMap<LSValue*,double>::ls_look},
		{Type::PTR_INT_MAP, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_look},
		{Type::INT_PTR_MAP, Type::POINTER, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_look},
		{Type::INT_FLOAT_MAP, Type::FLOAT, {Type::INTEGER, Type::FLOAT}, (void*) &LSMap<int,double>::ls_look},
		{Type::INT_INT_MAP, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_look},
	});
	*/
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


}

