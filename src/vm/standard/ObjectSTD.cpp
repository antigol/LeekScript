#include "ObjectSTD.hpp"
#include "../value/LSObject.hpp"


namespace ls {

//LSObject* object_map(const LSObject* object);

ObjectSTD::ObjectSTD() : Module("Object") {

	operator_("in", {
		{Type::OBJECT, Type::POINTER, Type::BOOLEAN, (void*) &LSObject::in}
	});

	//method("map", Type::OBJECT, Type::OBJECT, {}, (void*) &object_map);

	method("keys", Type::OBJECT, Type::STRING_ARRAY, {}, (void*) &LSObject::ls_get_keys);
	method("values", Type::OBJECT, Type::PTR_ARRAY, {}, (void*) &LSObject::ls_get_values);
}

/*
LSObject* object_map(const LSObject*) {

	LSObject* new_obj = new LSObject();

	return new_obj;
}
*/
}
