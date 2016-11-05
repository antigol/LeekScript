/*
 * IntervalSTD.cpp
 *
 *  Created on: 4 nov. 2016
 *      Author: pierre
 */

#include "IntervalSTD.hpp"

#include "../value/LSInterval.hpp"

namespace ls {

IntervalSTD::IntervalSTD() : Module("Interval") {

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::INTERVAL, Type::INTEGER, Type::BOOLEAN, (void*) &LSInterval::in_v}
	});

	/*
	 * Methods
	 */
	Type pred_fun_type_int = Type::FUNCTION;
	pred_fun_type_int.setArgumentType(0, Type::INTEGER);
	pred_fun_type_int.setReturnType(Type::BOOLEAN);

	method("filter", {
		{Type::INTERVAL, Type::INT_ARRAY, {pred_fun_type_int}, (void*) &LSInterval::ls_filter},
	});
}

IntervalSTD::~IntervalSTD() {}

}
