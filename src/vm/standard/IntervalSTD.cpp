#include "IntervalSTD.hpp"

#include "../value/LSInterval.hpp"

namespace ls {

IntervalSTD::IntervalSTD() : Module("Interval") {

	LSInterval::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::INTERVAL, Type::INTEGER, Type::BOOLEAN, (void*) &LSInterval::in_i, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	Type pred_fun_type_int = Type::FUNCTION_P;
	pred_fun_type_int.setArgumentType(0, Type::INTEGER);
	pred_fun_type_int.setReturnType(Type::BOOLEAN);

	method("filter", {
		{Type::INTERVAL, Type::INT_ARRAY, {pred_fun_type_int}, (void*) &LSInterval::ls_filter, Method::NATIVE},
	});

	method("sum", {
		{Type::INTERVAL, Type::LONG, {}, (void*) &LSInterval::ls_sum, Method::NATIVE},
	});
	method("product", {
		{Type::INTERVAL, Type::LONG, {}, (void*) &LSInterval::ls_product, Method::NATIVE},
	});
}

IntervalSTD::~IntervalSTD() {}

}
