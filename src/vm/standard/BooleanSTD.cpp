#include "BooleanSTD.hpp"


namespace ls {

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
BooleanSTD::BooleanSTD() : Module("Boolean") {

//	static_method("compare", Type::I32, {Type::BOOLEAN, Type::BOOLEAN}, (void*) &boolean_compare);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

int boolean_compare(bool a, bool b) {
	if (a) {
		if (not b) return 1;
	} else {
		if (b) return -1;
	}
	return 0;
}

}

