#include "NumberSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../../../lib/utf8.h"

using namespace std;

namespace ls {


jit_value_t Number_e(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, M_E);
}
jit_value_t Number_phi(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, 1.61803398874989484820);
}
jit_value_t Number_pi(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, 3.14159265358979323846);
}
jit_value_t Number_epsilon(jit_function_t F) {
	return jit_value_create_float64_constant(F, jit_type_float64, std::numeric_limits<double>::epsilon());
}

LSNumber* number_abs(LSNumber* x) {
	double r = fabs(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_acos(LSNumber* x) {
	double r = acos(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_asin(LSNumber* x) {
	double r = asin(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_atan(LSNumber* x) {
	double r = atan(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_atan2(LSNumber* x, LSNumber* y) {
	double r = atan2(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return new LSVar(r);
}

LSNumber* number_cbrt(LSNumber* x) {
	double r = cbrt(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_ceil(LSNumber* x) {
	double r = ceil(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_cos(LSNumber* x) {
	double r = cos(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_exp(LSNumber* x) {
	double r = exp(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_floor(LSNumber* x) {
	double r = floor(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_hypot(LSNumber* x, LSNumber* y) {
	double r = hypot(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return new LSVar(r);
}

LSNumber* number_log(LSNumber* x) {
	double r = log(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_log10(LSNumber* x) {
	double r = log10(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_max(LSNumber* x, LSNumber* y) {
	if (x->value < y->value) {
		if (x->refs == 0) delete x;
		return y;
	} else {
		if (y->refs == 0) delete y;
		return x;
	}
}

LSNumber* number_min(LSNumber* x, LSNumber* y) {
	if (x->value > y->value) {
		if (x->refs == 0) delete x;
		return y;
	} else {
		if (y->refs == 0) delete y;
		return x;
	}
}

LSNumber* number_pow(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y;
	}
	return new LSVar(r);
}

double number_rand() {
	return (double) rand() / RAND_MAX;
}

LSNumber* number_randFloat(LSNumber* min, LSNumber* max) {
	double r = min->value + ((double) rand() / RAND_MAX) * max->value;
	if (min->refs == 0) delete min;
	if (max->refs == 0) {
		max->value = r;
		return max;
	}
	return new LSVar(r);
}

int number_randInt(int min, int max) {
	return floor(min + ((double) rand() / RAND_MAX) * (max - min));
}

LSNumber* number_round(LSNumber* x) {
	double r = round(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_signum(LSNumber* x) {
	double r = 0;
	if (x->value > 0) r = 1;
	if (x->value < 0) r = -1;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_sin(LSNumber* x) {
	double r = sin(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_sqrt(LSNumber* x) {
	double r = sqrt(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_tan(LSNumber* x) {
	double r = tan(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_toDegrees(LSNumber* x) {
	double r = (x->value * 180) / M_PI;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

LSNumber* number_toRadians(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return new LSVar(r);
}

bool number_isInteger(LSNumber* x) {
	bool is = x->value == (int) x->value;
	if (x->refs == 0) delete x;
	return is;
}

LSString* number_char(LSNumber* x) {
	unsigned int n = x->value;
	if (x->refs == 0) delete x;
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
#endif
NumberSTD::NumberSTD() : Module("Number") {

	static_field("pi", Type::FLOAT, (void*) &Number_pi);
	static_field("e", Type::FLOAT, (void*) &Number_e);
	static_field("phi", Type::FLOAT, (void*) &Number_phi);
	static_field("epsilon", Type::FLOAT, (void*) &Number_epsilon);

	method("abs", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_abs);
	method("acos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_acos);
	method("asin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_asin);
	method("atan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_atan);
	method("cbrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cbrt);
	method("ceil", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_ceil);
	method("cos", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_cos);
	method("exp", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_exp);
	method("floor", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_floor);
	method("hypot", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_log10);

	method("max", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_max},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});

	method("min", {
		{Type::NUMBER, Type::NUMBER, {Type::NUMBER}, (void*) &number_min},
		{Type::FLOAT, Type::FLOAT, {Type::FLOAT}, nullptr},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, nullptr},
	});

	method("pow", Type::NUMBER, Type::FLOAT_P, {Type::NUMBER}, (void*) &number_pow);
	method("round", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_round);
	method("signum", Type::NUMBER, Type::INTEGER_P, {}, (void*) &number_signum);
	method("sin", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sin);
	method("sqrt", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_sqrt);
	method("tan", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_tan);
	method("toDegrees", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::FLOAT_P, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);
	method("char", Type::NUMBER, Type::STRING, {}, (void*) &number_char);

	static_method("abs", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_abs);
	static_method("acos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_acos);
	static_method("asin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_asin);
	static_method("atan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_atan);
	static_method("atan2", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_atan2);
	static_method("cbrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cbrt);
	static_method("ceil", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_ceil);
	static_method("cos", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_cos);
	static_method("exp", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_exp);
	static_method("floor", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_floor);
	static_method("hypot", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_log10);
	static_method("max", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_max);
	static_method("min", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_min);
	static_method("pow", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::FLOAT, {}, (void*) &number_rand);
	static_method("randFloat", Type::FLOAT_P, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);
	static_method("round", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_round);
	static_method("signum", Type::INTEGER_P, {Type::NUMBER}, (void*) &number_signum);
	static_method("sin", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sin);
	static_method("sqrt", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_sqrt);
	static_method("tan", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_tan);
	static_method("toDegrees", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::FLOAT_P, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);
	static_method("char", Type::STRING, {Type::NUMBER}, (void*) &number_char);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}

