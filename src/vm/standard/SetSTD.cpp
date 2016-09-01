#include "SetSTD.hpp"
#include "../value/LSSet.hpp"


using namespace std;

namespace ls {
/*
int set_size(const LSSet<LSValue*>* set) {
	int r = set->size();
	if (set->refs == 0) delete set;
	return r;
}
*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
SetSTD::SetSTD() : Module("Set") {
/*
	method("size", {
			   {Type::PTR_SET, Type::INTEGER, {}, (void*) set_size},
			   {Type::FLOAT_SET, Type::INTEGER, {}, (void*) set_size},
			   {Type::INT_SET, Type::INTEGER, {}, (void*) set_size},
		   });

	method("insert", {
			   {Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_insert},
			   {Type::FLOAT_SET, Type::BOOLEAN, {Type::FLOAT}, (void*) &LSSet<double>::ls_insert},
			   {Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_insert},
		   });

	method("clear", {
			   {Type::PTR_SET, Type::PTR_SET, {}, (void*) &LSSet<LSValue*>::ls_clear},
			   {Type::FLOAT_SET, Type::FLOAT_SET, {}, (void*) &LSSet<double>::ls_clear},
			   {Type::INT_SET, Type::INT_SET, {}, (void*) &LSSet<int>::ls_clear},
		   });

	method("erase", {
			   {Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_erase},
			   {Type::FLOAT_SET, Type::BOOLEAN, {Type::FLOAT}, (void*) &LSSet<double>::ls_erase},
			   {Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_erase},
		   });

	method("contains", {
			   {Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_contains},
			   {Type::FLOAT_SET, Type::BOOLEAN, {Type::FLOAT}, (void*) &LSSet<double>::ls_contains},
			   {Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_contains},
		   });
		   */
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}

