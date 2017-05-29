#include "SetSTD.hpp"
#include "../value/LSSet.hpp"

using namespace std;

namespace ls {

SetSTD::SetSTD() : Module("Set") {

	LSSet<LSValue*>::clazz = clazz;
	LSSet<int>::clazz = clazz;
	LSSet<double>::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::PTR_SET, Type::POINTER, Type::BOOLEAN, (void*) &LSSet<LSValue*>::in, Method::NATIVE},
		{Type::REAL_SET, Type::REAL, Type::BOOLEAN, (void*) &LSSet<double>::in, Method::NATIVE},
		{Type::INT_SET, Type::INTEGER, Type::BOOLEAN, (void*) &LSSet<int>::in, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("size", {
		{Type::PTR_SET, Type::INTEGER, {}, (void*) &LSSet<LSValue*>::ls_size, Method::NATIVE},
		{Type::REAL_SET, Type::INTEGER, {}, (void*) &LSSet<double>::ls_size, Method::NATIVE},
		{Type::INT_SET, Type::INTEGER, {}, (void*) &LSSet<int>::ls_size, Method::NATIVE},
	});
	method("insert", {
		{Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_insert, Method::NATIVE},
		{Type::REAL_SET, Type::BOOLEAN, {Type::REAL}, (void*) &LSSet<double>::ls_insert, Method::NATIVE},
		{Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_insert, Method::NATIVE},
	});
	method("clear", {
		{Type::PTR_SET, Type::PTR_SET, {}, (void*) &LSSet<LSValue*>::ls_clear, Method::NATIVE},
		{Type::REAL_SET, Type::REAL_SET, {}, (void*) &LSSet<double>::ls_clear, Method::NATIVE},
		{Type::INT_SET, Type::INT_SET, {}, (void*) &LSSet<int>::ls_clear, Method::NATIVE},
	});
	method("erase", {
		{Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_erase, Method::NATIVE},
		{Type::REAL_SET, Type::BOOLEAN, {Type::REAL}, (void*) &LSSet<double>::ls_erase, Method::NATIVE},
		{Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_erase, Method::NATIVE},
	});
	method("contains", {
		{Type::PTR_SET, Type::BOOLEAN, {Type::POINTER}, (void*) &LSSet<LSValue*>::ls_contains, Method::NATIVE},
		{Type::REAL_SET, Type::BOOLEAN, {Type::REAL}, (void*) &LSSet<double>::ls_contains, Method::NATIVE},
		{Type::INT_SET, Type::BOOLEAN, {Type::INTEGER}, (void*) &LSSet<int>::ls_contains, Method::NATIVE},
	});

	/*
	 * Static methods
	 */
	static_method("size", {
		{Type::INTEGER, {Type::PTR_SET}, (void*) &LSSet<LSValue*>::ls_size, Method::NATIVE},
		{Type::INTEGER, {Type::REAL_SET}, (void*) &LSSet<double>::ls_size, Method::NATIVE},
		{Type::INTEGER, {Type::INT_SET}, (void*) &LSSet<int>::ls_size, Method::NATIVE},
	});
}

}
