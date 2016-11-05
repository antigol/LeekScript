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

	operator_("in", {
		{Type::PTR_PTR_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, LSValue*>::in},
		{Type::PTR_REAL_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, double>::in},
		{Type::PTR_INT_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, int>::in},
		{Type::REAL_PTR_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, LSValue*>::in},
		{Type::REAL_REAL_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, double>::in},
		{Type::REAL_INT_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, int>::in},
		{Type::INT_PTR_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, LSValue*>::in},
		{Type::INT_REAL_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, double>::in},
		{Type::INT_INT_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, int>::in},
	});

	method("size", {
		{Type::PTR_PTR_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::PTR_REAL_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::PTR_INT_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_PTR_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_REAL_MAP, Type::INTEGER, {}, (void*) map_size},
		{Type::INT_INT_MAP, Type::INTEGER, {}, (void*) map_size},
    });

	method("values", {
		{Type::PTR_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<LSValue*, LSValue*>::values},
		{Type::PTR_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<LSValue*, double>::values},
		{Type::PTR_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<LSValue*, int>::values},
		{Type::REAL_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<double, LSValue*>::values},
		{Type::REAL_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<double, double>::values},
		{Type::REAL_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<double, int>::values},
		{Type::INT_PTR_MAP, Type::PTR_ARRAY, {}, (void*) &LSMap<int, LSValue*>::values},
		{Type::INT_REAL_MAP, Type::REAL_ARRAY, {}, (void*) &LSMap<int, double>::values},
		{Type::INT_INT_MAP, Type::INT_ARRAY, {}, (void*) &LSMap<int, int>::values}
	});

	method("insert", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_insert},
		{Type::PTR_REAL_MAP, Type::BOOLEAN, {Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_insert},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_insert},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_insert},
		{Type::INT_REAL_MAP, Type::BOOLEAN, {Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_insert},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_insert},
    });

	method("clear", {
		{Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear},
		{Type::PTR_REAL_MAP, Type::PTR_REAL_MAP, {}, (void*) &LSMap<LSValue*,double>::ls_clear},
		{Type::PTR_INT_MAP, Type::PTR_INT_MAP, {}, (void*) &LSMap<LSValue*,int>::ls_clear},
		{Type::INT_PTR_MAP, Type::INT_PTR_MAP, {}, (void*) &LSMap<int,LSValue*>::ls_clear},
		{Type::INT_REAL_MAP, Type::INT_REAL_MAP, {}, (void*) &LSMap<int,double>::ls_clear},
		{Type::INT_INT_MAP, Type::INT_INT_MAP, {}, (void*) &LSMap<int,int>::ls_clear},
	});

	method("erase", {
		{Type::PTR_PTR_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase},
		{Type::PTR_REAL_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,double>::ls_erase},
		{Type::PTR_INT_MAP, Type::BOOLEAN, {Type::POINTER}, (void*) &LSMap<LSValue*,int>::ls_erase},
		{Type::INT_PTR_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,LSValue*>::ls_erase},
		{Type::INT_REAL_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,double>::ls_erase},
		{Type::INT_INT_MAP, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSMap<int,int>::ls_erase},
	});

	method("look", {
		{Type::PTR_PTR_MAP, Type::POINTER, {Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_look},
		{Type::PTR_REAL_MAP, Type::REAL, {Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_look},
		{Type::PTR_INT_MAP, Type::INTEGER, {Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_look},
		{Type::INT_PTR_MAP, Type::POINTER, {Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_look},
		{Type::INT_REAL_MAP, Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_look},
		{Type::INT_INT_MAP, Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_look},
	});
}

}
