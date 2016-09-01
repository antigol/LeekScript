#include "ObjectSTD.hpp"

namespace ls {
/*
LSObject* object_map(const LSObject*) {

	LSObject* new_obj = new LSObject();

	return new_obj;
}
*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
ObjectSTD::ObjectSTD() : Module("Object") {
/*
	method("map", Type::OBJECT, Type::OBJECT, {}, (void*) &object_map);

	method("keys", Type::OBJECT, Type::STRING_ARRAY, {}, (void*) &LSObject::ls_get_keys);
	method("values", Type::OBJECT, Type::PTR_ARRAY, {}, (void*) &LSObject::ls_get_values);
	*/
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}

