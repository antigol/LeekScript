#include "NumberSTD.hpp"

#include "../value/LSNumber.hpp"
#include "../../../lib/utf8.h"
#include "../../compiler/Compiler.hpp"

using namespace std;

namespace ls {

int mpz_log(__mpz_struct n) {
	int l = abs(n._mp_size) - 1;
	unsigned long s = n._mp_d[l];
	unsigned char r = 0;
	while (s >>= 1) r++;
	return 64 * l + r + 1;
}

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

double number_hypot(LSNumber* x, LSNumber* y) {
	double r = hypot(x->value, y->value);
	if (x->refs == 0) delete x;
	if (y->refs == 0) {
		y->value = r;
		return y->value;
	}
	return r;
}

double number_log(LSNumber* x) {
	double r = log(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_log10(LSNumber* x) {
	double r = log10(x->value);
	if (x->refs == 0) {
		x->value = r;
		return x->value;
	}
	return r;
}

double number_pow(LSNumber* x, LSNumber* y) {
	double r = pow(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return r;
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
	return LSNumber::get(r);
}

int number_randInt(int min, int max) {
	return floor(min + ((double) rand() / RAND_MAX) * (max - min));
}

LSNumber* number_signum(LSNumber* x) {
	double r = 0;
	if (x->value > 0) r = 1;
	if (x->value < 0) r = -1;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_toDegrees(LSNumber* x) {
	double r = (x->value * 180) / M_PI;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

LSNumber* number_toRadians(LSNumber* x) {
	double r = (x->value * M_PI) / 180;
	if (x->refs == 0) {
		x->value = r;
		return x;
	}
	return LSNumber::get(r);
}

bool number_isInteger(LSNumber* x) {

	bool is = x->value == (int) x->value;
	if (x->refs == 0) delete x;
	return is;
}

NumberSTD::NumberSTD() : Module("Number") {

	operator_("=", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT, (void*) &NumberSTD::store_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::GMP_INT, (void*) &NumberSTD::store_gmp_gmp_tmp, Method::NATIVE},
	});

	operator_("+", {
		{Type::INTEGER, Type::POINTER, Type::POINTER, (void*) &NumberSTD::add_int_ptr},
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::add_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::add_gmp_tmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::add_gmp_gmp_tmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::add_gmp_tmp_gmp_tmp, Method::NATIVE},
		{Type::REAL, Type::REAL, Type::REAL, (void*) &NumberSTD::add_real_real, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, Type::INTEGER, (void*) &NumberSTD::add_real_real, Method::NATIVE},
	});

	operator_("+=", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT, (void*) &NumberSTD::add_eq_gmp_gmp, Method::NATIVE}
	});

	operator_("-", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_tmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_gmp_tmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_tmp_gmp_tmp, Method::NATIVE},
		{Type::GMP_INT, Type::INTEGER, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_int, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::INTEGER, Type::GMP_INT_TMP, (void*) &NumberSTD::sub_gmp_tmp_int, Method::NATIVE}
	});

	operator_("*", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::mul_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::mul_gmp_tmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::mul_gmp_gmp_tmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::mul_gmp_tmp_gmp_tmp, Method::NATIVE},
	});

	operator_("**", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::pow_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::INTEGER, Type::GMP_INT_TMP, (void*) &NumberSTD::pow_gmp_int, Method::NATIVE},
	});

	operator_("<", {
		{Type::NUMBER_VALUE, Type::NUMBER_VALUE, Type::BOOLEAN, (void*) &NumberSTD::lt, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT, Type::BOOLEAN, (void*) &NumberSTD::lt_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT_TMP, Type::BOOLEAN, (void*) &NumberSTD::lt_gmp_tmp_gmp_tmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT, Type::BOOLEAN, (void*) &NumberSTD::lt_gmp_tmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::BOOLEAN, (void*) &NumberSTD::lt_gmp_gmp_tmp, Method::NATIVE},
	});

	operator_(">", {
		{Type::NUMBER_VALUE, Type::NUMBER_VALUE, Type::BOOLEAN, (void*) &NumberSTD::gt, Method::NATIVE},
		{Type::INTEGER, Type::GMP_INT, Type::BOOLEAN, (void*) &NumberSTD::gt_int_gmp, Method::NATIVE}
	});

	operator_("%", {
		{Type::GMP_INT, Type::GMP_INT, Type::GMP_INT_TMP, (void*) &NumberSTD::mod_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::GMP_INT_TMP, Type::GMP_INT_TMP, (void*) &NumberSTD::mod_gmp_gmp_tmp, Method::NATIVE},
	});

	operator_("==", {
		{Type::GMP_INT, Type::GMP_INT, Type::BOOLEAN, (void*) &NumberSTD::eq_gmp_gmp, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::GMP_INT, Type::BOOLEAN, (void*) &NumberSTD::eq_gmp_tmp_gmp, Method::NATIVE},
		{Type::GMP_INT, Type::INTEGER, Type::BOOLEAN, (void*) &NumberSTD::eq_gmp_int, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::INTEGER, Type::BOOLEAN, (void*) &NumberSTD::eq_gmp_tmp_int, Method::NATIVE},
	});

	Type tilde_fun_type_int = Type::FUNCTION_P;
	tilde_fun_type_int.setArgumentType(0, Type::INTEGER);
	tilde_fun_type_int.setReturnType(Type::POINTER);
	Type tilde_fun_type_real = Type::FUNCTION_P;
	tilde_fun_type_real.setArgumentType(0, Type::REAL);
	tilde_fun_type_real.setReturnType(Type::POINTER);
	operator_("~", {
		{Type::REAL, tilde_fun_type_real, Type::POINTER, (void*) &NumberSTD::tilde_real, Method::NATIVE},
		{Type::INTEGER, tilde_fun_type_int, Type::POINTER, (void*) &NumberSTD::tilde_int, Method::NATIVE}
	});

	static_field("pi", Type::REAL, (void*) &Number_pi);
	static_field("e", Type::REAL, (void*) &Number_e);
	static_field("phi", Type::REAL, (void*) &Number_phi);
	static_field("epsilon", Type::REAL, (void*) &Number_epsilon);

	/*
	 * Methods
	 */
	method("abs", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::abs_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});
	method("acos", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::acos_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::acos_real, Method::NATIVE},
	});
	method("asin", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::asin_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::asin_real, Method::NATIVE},
	});
	method("atan", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::atan_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::atan_real, Method::NATIVE},
	});
	method("atan2", {
		{Type::POINTER, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::atan2_ptr_ptr},
		{Type::POINTER, Type::REAL, {Type::REAL}, (void*) &NumberSTD::atan2_ptr_real},
		{Type::REAL, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::atan2_real_ptr},
		{Type::REAL, Type::REAL, {Type::REAL}, (void*) &NumberSTD::atan2_real_real, Method::NATIVE}
	});
	method("cbrt", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::cbrt_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::cbrt_real, Method::NATIVE}
	});
	method("ceil", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::ceil_ptr}
	});
	method("char", {
		{Type::NUMBER, Type::POINTER, {}, (void*) &NumberSTD::char_ptr},
		{Type::REAL, Type::STRING, {}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::INTEGER, Type::STRING, {}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});
	method("cos", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::cos_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::cos_real, Method::NATIVE}
	});
	method("exp", {
		{Type::POINTER, Type::REAL, {}, (void*) &NumberSTD::exp_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::exp_real, Method::NATIVE}
	});
	method("floor", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::floor_ptr},
		{Type::REAL, Type::INTEGER, {}, (void*) &NumberSTD::floor_real, Method::NATIVE}
	});
	method("hypot", Type::NUMBER, Type::REAL, {Type::NUMBER}, (void*) &number_hypot);
	method("log", Type::NUMBER, Type::REAL, {}, (void*) &number_log);
	method("log10", Type::NUMBER, Type::REAL, {}, (void*) &number_log10);
	method("max", {
		{Type::POINTER, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::REAL, Type::REAL, {Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
	});
	method("min", {
		{Type::POINTER, Type::REAL, {Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::REAL, Type::REAL, {Type::REAL}, (void*) &NumberSTD::min_float_float, Method::NATIVE},
		{Type::INTEGER, Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::min_float_float, Method::NATIVE},
	});
	method("pow", {
		{Type::NUMBER, Type::REAL, {Type::NUMBER}, (void*) &number_pow},
		{Type::LONG, Type::LONG, {Type::INTEGER}, (void*) &NumberSTD::pow_int, Method::NATIVE}
	});
	method("round", {
		{Type::NUMBER, Type::INTEGER, {}, (void*) &NumberSTD::round_ptr}
	});
	method("signum", Type::NUMBER, Type::INTEGER, {}, (void*) &number_signum);
	method("sin", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::sin_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::sin_real, Method::NATIVE}
	});
	method("sqrt", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::sqrt_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::sqrt_real, Method::NATIVE}
	});
	method("tan", {
		{Type::NUMBER, Type::REAL, {}, (void*) &NumberSTD::tan_ptr},
		{Type::REAL, Type::REAL, {}, (void*) &NumberSTD::tan_real, Method::NATIVE}
	});
	method("toDegrees", Type::NUMBER, Type::REAL, {}, (void*) &number_toDegrees);
	method("toRadians", Type::NUMBER, Type::REAL, {}, (void*) &number_toRadians);
	method("isInteger", Type::NUMBER, Type::BOOLEAN, {}, (void*) &number_isInteger);

	method("isPrime", {
		{Type::GMP_INT, Type::INTEGER, {}, (void*) &NumberSTD::is_prime, Method::NATIVE},
		{Type::GMP_INT_TMP, Type::INTEGER, {}, (void*) &NumberSTD::is_prime_tmp, Method::NATIVE},
		{Type::LONG, Type::BOOLEAN, {}, (void*) &NumberSTD::is_prime_long, Method::NATIVE},
		{Type::INTEGER, Type::BOOLEAN, {}, (void*) &NumberSTD::is_prime_int, Method::NATIVE}
	});

	/*
	 * Static methods
	 */
	static_method("abs", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::abs_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::abs_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::abs_real, Method::NATIVE}
	});
	static_method("acos", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::acos_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::acos_real, Method::NATIVE},
	});
	static_method("asin", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::asin_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::asin_real, Method::NATIVE},
	});
	static_method("atan", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::atan_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::atan_real, Method::NATIVE},
	});
	static_method("atan2", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::atan2_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::atan2_ptr_real},
		{Type::REAL, {Type::REAL, Type::POINTER}, (void*) &NumberSTD::atan2_real_ptr},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::atan2_real_real, Method::NATIVE}
	});
	static_method("cbrt", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::cbrt_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::cbrt_real, Method::NATIVE}
	});
	static_method("ceil", {
		{Type::REAL, {Type::NUMBER}, (void*) &NumberSTD::ceil_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::ceil_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::ceil_int, Method::NATIVE}
	});
	static_method("char", {
		{Type::STRING, {Type::POINTER}, (void*) &NumberSTD::char_ptr},
		{Type::STRING, {Type::REAL}, (void*) &NumberSTD::char_real, Method::NATIVE},
		{Type::STRING, {Type::INTEGER}, (void*) &NumberSTD::char_int, Method::NATIVE}
	});
	static_method("cos", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::cos_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::cos_real, Method::NATIVE},
	});
	static_method("exp", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::exp_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::exp_real, Method::NATIVE},
	});
	static_method("floor", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::floor_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::floor_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::floor_int, Method::NATIVE},
	});
	static_method("hypot", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_hypot);
	static_method("log", Type::REAL, {Type::NUMBER}, (void*) &number_log);
	static_method("log10", Type::REAL, {Type::NUMBER}, (void*) &number_log10);

	static_method("max", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::max_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::max_ptr_float},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::max_ptr_int},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::max_float_float, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::max_float_float, Method::NATIVE}
	});
	static_method("min", {
		{Type::REAL, {Type::POINTER, Type::POINTER}, (void*) &NumberSTD::min_ptr_ptr},
		{Type::REAL, {Type::POINTER, Type::REAL}, (void*) &NumberSTD::min_ptr_float},
		{Type::REAL, {Type::POINTER, Type::INTEGER}, (void*) &NumberSTD::min_ptr_int},
		{Type::REAL, {Type::REAL, Type::REAL}, (void*) &NumberSTD::min_float_float, Method::NATIVE},
		{Type::REAL, {Type::REAL, Type::INTEGER}, (void*) &NumberSTD::min_float_float, Method::NATIVE},
		{Type::REAL, {Type::INTEGER, Type::REAL}, (void*) &NumberSTD::min_float_float, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &NumberSTD::min_float_float, Method::NATIVE}
	});
	static_method("pow", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_pow);
	static_method("rand", Type::REAL, {}, (void*) &number_rand);
	static_method("randFloat", Type::REAL, {Type::NUMBER, Type::NUMBER}, (void*) &number_randFloat);
	static_method("randInt", Type::INTEGER, {Type::INTEGER, Type::INTEGER}, (void*) &number_randInt);

	static_method("round", {
		{Type::INTEGER, {Type::POINTER}, (void*) &NumberSTD::round_ptr},
		{Type::INTEGER, {Type::REAL}, (void*) &NumberSTD::round_real, Method::NATIVE},
		{Type::INTEGER, {Type::INTEGER}, (void*) &NumberSTD::round_int, Method::NATIVE}
	});

	static_method("signum", Type::INTEGER, {Type::NUMBER}, (void*) &number_signum);

	static_method("sin", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sin_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::sin_real, Method::NATIVE},
	});

	static_method("sqrt", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::sqrt_ptr}
	});

	static_method("tan", {
		{Type::REAL, {Type::POINTER}, (void*) &NumberSTD::tan_ptr},
		{Type::REAL, {Type::REAL}, (void*) &NumberSTD::tan_real, Method::NATIVE},
	});
	static_method("toDegrees", Type::REAL, {Type::NUMBER}, (void*) &number_toDegrees);
	static_method("toRadians", Type::REAL, {Type::NUMBER}, (void*) &number_toRadians);
	static_method("isInteger", Type::BOOLEAN, {Type::NUMBER}, (void*) &number_isInteger);
}

Compiler::value NumberSTD::store_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID,{a, b}, &mpz_set);
	return args[0];
}

Compiler::value NumberSTD::store_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	VM::delete_gmp_int(c.F, args[0].v);
	jit_insn_store(c.F, args[0].v, args[1].v);
	return args[1];
}

Compiler::value NumberSTD::add_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_add(args[0], args[1]);
}

LSValue* NumberSTD::add_int_ptr(int a, LSValue* b) {
	return b->ls_radd(LSNumber::get(a));
}

Compiler::value NumberSTD::add_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.new_mpz();
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_add);
	return r;
}

Compiler::value NumberSTD::add_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT, args,	+[](__mpz_struct a, __mpz_struct b) {
		mpz_add(&b, &a, &b);
		return b;
	});
}

Compiler::value NumberSTD::add_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_add);
	return args[0];
}

Compiler::value NumberSTD::add_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_add);
	VM::delete_gmp_int(c.F, args[1].v);
	return args[0];
}

Compiler::value NumberSTD::add_eq_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_add);
	return args[0];
}

Compiler::value NumberSTD::sub_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT, args, +[](__mpz_struct a, __mpz_struct b) {
		mpz_t res;
		mpz_init(res);
		mpz_sub(res, &a, &b);
		return *res;
	});
}
Compiler::value NumberSTD::sub_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT, args, +[](__mpz_struct a, __mpz_struct b) {
		mpz_sub(&b, &a, &b);
		return b;
	});
}

Compiler::value NumberSTD::sub_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_sub);
	return args[0];
}

Compiler::value NumberSTD::sub_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a, a, b}, &mpz_sub);
	VM::delete_gmp_int(c.F, args[1].v);
	return args[0];
}

Compiler::value NumberSTD::sub_gmp_int(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = args[1];
	auto r = c.new_mpz();
	auto r_addr = c.insn_address_of(r);

	jit_label_t label_end = jit_label_undefined;
	jit_label_t label_else = jit_label_undefined;

	auto cond = c.insn_lt(b, c.new_integer(0));
	jit_insn_branch_if_not(c.F, cond.v, &label_else);

	Compiler::value neg_b = {jit_insn_neg(c.F, b.v), Type::INTEGER};
	c.insn_call(Type::VOID, {r_addr, a, neg_b}, &mpz_add_ui);
	jit_insn_branch(c.F, &label_end);

	jit_insn_label(c.F, &label_else);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_sub_ui);
	jit_insn_label(c.F, &label_end);
	return r;
}

Compiler::value NumberSTD::sub_gmp_tmp_int(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = args[1];
	c.insn_call(Type::VOID, {a, a, b}, &mpz_sub_ui);
	VM::delete_gmp_int(c.F, args[1].v);
	return args[0];
}

void mpz_mul_custom(__mpz_struct* r, __mpz_struct* a, __mpz_struct* b) {
//	std::cout << "size a: " << mpz_log(*a) << std::endl;
//	std::cout << "size b: " << mpz_log(*b) << std::endl;
//	std::cout << "expected size: " << mpz_log(*a) + mpz_log(*b) << std::endl;
	mpz_mul(r, a, b);
//	std::cout << "size r: " << mpz_log(*r) << std::endl;
}

Compiler::value NumberSTD::mul_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	Compiler::value r = {VM::create_gmp_int(c.F), Type::GMP_INT};
	auto r_addr = c.insn_address_of(r);
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {r_addr, a, b}, &mpz_mul_custom);
	return r;
}

Compiler::value NumberSTD::mul_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_address_of(args[0]);
	auto b = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a, a, b}, &mpz_mul);
	VM::delete_gmp_int(c.F, args[1].v);
	return args[0];
}

Compiler::value NumberSTD::mul_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT, args, +[](__mpz_struct a, __mpz_struct b) {
		mpz_mul(&b, &a, &b);
		return b;
	});
}

Compiler::value NumberSTD::mul_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {a_addr, a_addr, b_addr}, &mpz_mul);
	return args[0];
}

Compiler::value NumberSTD::pow_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT, args, +[](__mpz_struct a, __mpz_struct b) {
		mpz_t res;
		mpz_init(res);
		mpz_pow_ui(res, &a, mpz_get_ui(&b));
		return *res;
	});
}

__mpz_struct pow_gmp_int_lambda(__mpz_struct a, int b) throw() {
	mpz_t res;
	mpz_init(res);
	mpz_pow_ui(res, &a, b);
	return *res;
}

Compiler::value NumberSTD::pow_gmp_int(Compiler& c, std::vector<Compiler::value> args) {
	// Check: mpz_log(a) * b <= 10000
	auto a_size = c.insn_call(Type::INTEGER, {args[0]}, (void*) &mpz_log);
	auto r_size = c.insn_mul(a_size, args[1]);
	auto cond = c.insn_lt(r_size, c.new_integer(10000));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if(c.F, cond.v, &label_end);
	jit_insn_throw(c.F, LS_CREATE_INTEGER(c.F, VM::Exception::NUMBER_OVERFLOW));
	jit_insn_label(c.F, &label_end);

	// Ops: size of the theorical result
	VM::inc_ops_jit(c.F, r_size.v);

	VM::inc_gmp_counter(c.F);
	return c.insn_call(Type::GMP_INT_TMP, args, &pow_gmp_int_lambda);
}

Compiler::value NumberSTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_lt(args[0], args[1]);
}

Compiler::value NumberSTD::lt_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp);
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::lt_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp);
	VM::delete_gmp_int(c.F, args[0].v);
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::lt_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args, +[](__mpz_struct a, __mpz_struct b) {
		bool res = mpz_cmp(&a, &b) < 0;
		mpz_clear(&b);
		VM::gmp_values_deleted++;
		return res;
	});
}
Compiler::value NumberSTD::lt_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp);
	VM::delete_gmp_int(c.F, args[0].v);
	VM::delete_gmp_int(c.F, args[1].v);
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::gt(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_gt(args[0], args[1]);
}

Compiler::value NumberSTD::gt_int_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {b_addr, args[0]}, &_mpz_cmp_si);
	return c.insn_lt(res, c.new_integer(0));
}

Compiler::value NumberSTD::mod_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::GMP_INT_TMP, args, +[](__mpz_struct a, __mpz_struct b) {
		mpz_t res;
//		mpz_init(res);
		mpz_mod(res, &a, &b);
		return *res;
	});
}
Compiler::value NumberSTD::mod_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	c.insn_call(Type::VOID, {b_addr, a_addr, b_addr}, &mpz_mod);
	return args[1];
}

Compiler::value NumberSTD::eq_gmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args,
	+[](__mpz_struct a, __mpz_struct b) {
		return mpz_cmp(&a, &b) == 0;
	});
}
Compiler::value NumberSTD::eq_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto b_addr = c.insn_address_of(args[1]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, b_addr}, &mpz_cmp);
	VM::delete_gmp_int(c.F, args[0].v);
	return c.insn_eq(res, c.new_integer(0));
}
Compiler::value NumberSTD::eq_gmp_int(Compiler& c, std::vector<Compiler::value> args) {
	auto a_addr = c.insn_address_of(args[0]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, args[1]}, &_mpz_cmp_si);
	return c.insn_eq(res, c.new_integer(0));
}
Compiler::value NumberSTD::eq_gmp_tmp_int(Compiler& c, std::vector<Compiler::value> args) {
	Compiler::value a_addr = c.insn_address_of(args[0]);
	auto res = c.insn_call(Type::INTEGER, {a_addr, args[1]}, &_mpz_cmp_si);
	VM::delete_gmp_int(c.F, args[0].v);
	return c.insn_eq(res, c.new_integer(0));
}

Compiler::value NumberSTD::tilde_int(Compiler& c, std::vector<Compiler::value> args) {
	jit_type_t arg_types[] = {LS_POINTER, LS_INTEGER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, arg_types, 2, 0);
	auto i = args[0].v;
	auto f = args[1].v;
	auto fun = jit_insn_load_relative(c.F, f, 16, LS_POINTER);
	jit_value_t jit_args[] = {f, i};
	return {jit_insn_call_indirect(c.F, fun, sig, jit_args, 2, 0), Type::POINTER};
}

Compiler::value NumberSTD::tilde_real(Compiler& c, std::vector<Compiler::value> args) {
	jit_type_t arg_types[] = {LS_POINTER, LS_REAL};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, arg_types, 2, 0);
	auto r = args[0].v;
	auto f = args[1].v;
	auto fun = jit_insn_load_relative(c.F, f, 16, LS_POINTER);
	jit_value_t jit_args[] = {f, r};
	return {jit_insn_call_indirect(c.F, fun, sig, jit_args, 2, 0), Type::POINTER};
}

/*
 * Methods
 */

double NumberSTD::abs_ptr(LSNumber* x) {
	// TODO check args
	// VM::check_arg(args, {LS_NUMBER})
	// Number.abs(["hello", 12][0])
	// ==>  Execution error Number.abs(number x) : first parameter 'x' is not a number but a string.

	double a = fabs(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::abs_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_abs(c.F, args[0].v), Type::REAL};
}

double NumberSTD::acos_ptr(LSNumber* x) {
	double a = acos(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::acos_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_acos(c.F, args[0].v), Type::REAL};
}

double NumberSTD::asin_ptr(LSNumber* x) {
	double a = asin(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::asin_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_asin(c.F, args[0].v), Type::REAL};
}

double NumberSTD::atan_ptr(LSNumber* x) {
	double a = atan(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::atan_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_atan(c.F, args[0].v), Type::REAL};
}

double NumberSTD::atan2_ptr_ptr(LSNumber* y, LSNumber* x) {
	double a = atan2(y->value, x->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return a;
}
double NumberSTD::atan2_ptr_real(LSNumber* y, double x) {
	double a = atan2(y->value, x);
	LSValue::delete_temporary(y);
	return a;
}
double NumberSTD::atan2_real_ptr(double y, LSNumber* x) {
	double a = atan2(y, x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::atan2_real_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_atan2(c.F, args[0].v, args[1].v), Type::REAL};
}

LSString* NumberSTD::char_ptr(LSNumber* x) {
	unsigned int n = x->value;
	LSValue::delete_temporary(x);
	char dest[5];
	u8_toutf8(dest, 5, &n, 1);
	return new LSString(dest);
}
Compiler::value NumberSTD::char_real(Compiler& c, vector<Compiler::value> args) {
	return c.insn_call(Type::STRING, args, +[](double x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}
Compiler::value NumberSTD::char_int(Compiler& c, vector<Compiler::value> args) {
	return c.insn_call(Type::STRING, args, +[](int x) {
		unsigned int n = x;
		char dest[5];
		u8_toutf8(dest, 5, &n, 1);
		return new LSString(dest);
	});
}

double NumberSTD::exp_ptr(LSNumber* x) {
	double a = exp(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::exp_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_exp(c.F, args[0].v), Type::REAL};
}

int NumberSTD::floor_ptr(LSNumber* x) {
	int a = floor(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::floor_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_floor(c.F, args[0].v), Type::REAL};
}
Compiler::value NumberSTD::floor_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::round_ptr(LSNumber* x) {
	int a = round(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::round_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_round(c.F, args[0].v), Type::REAL};
}
Compiler::value NumberSTD::round_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

int NumberSTD::ceil_ptr(LSNumber* x) {
	int a = ceil(x->value);
	LSValue::delete_temporary(x);
	return a;
}
Compiler::value NumberSTD::ceil_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_ceil(c.F, args[0].v), Type::REAL};
}
Compiler::value NumberSTD::ceil_int(Compiler&, std::vector<Compiler::value> args) {
	return args[0]; // Nothing to do
}

double NumberSTD::max_ptr_ptr(LSNumber* x, LSNumber* y) {
	int max = fmax(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_ptr_float(LSNumber* x, double y) {
	int max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_ptr_int(LSNumber* x, int y) {
	int max = fmax(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::max_float_ptr(double x, LSNumber* y) {
	int max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::max_int_ptr(int x, LSNumber* y) {
	int max = fmax(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
Compiler::value NumberSTD::max_float_float(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_max(c.F, args[0].v, args[1].v), Type::REAL};
}

double NumberSTD::min_ptr_ptr(LSNumber* x, LSNumber* y) {
	int max = fmin(x->value, y->value);
	LSValue::delete_temporary(x);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::min_ptr_float(LSNumber* x, double y) {
	int max = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::min_ptr_int(LSNumber* x, int y) {
	int max = fmin(x->value, y);
	LSValue::delete_temporary(x);
	return max;
}
double NumberSTD::min_float_ptr(double x, LSNumber* y) {
	int max = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
double NumberSTD::min_int_ptr(int x, LSNumber* y) {
	int max = fmin(x, y->value);
	LSValue::delete_temporary(y);
	return max;
}
Compiler::value NumberSTD::min_float_float(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_min(c.F, args[0].v, args[1].v), Type::REAL};
}

double NumberSTD::cos_ptr(LSNumber* x) {
	double c = cos(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::cos_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_cos(c.F, args[0].v), Type::REAL};
}

double NumberSTD::sin_ptr(LSNumber* x) {
	double s = sin(x->value);
	LSValue::delete_temporary(x);
	return s;
}
Compiler::value NumberSTD::sin_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_sin(c.F, args[0].v), Type::REAL};
}

double NumberSTD::tan_ptr(LSNumber* x) {
	double c = tan(x->value);
	LSValue::delete_temporary(x);
	return c;
}
Compiler::value NumberSTD::tan_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_tan(c.F, args[0].v), Type::REAL};
}

double NumberSTD::sqrt_ptr(LSNumber* x) {
	double s = sqrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

Compiler::value NumberSTD::sqrt_real(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_sqrt(c.F, args[0].v), Type::REAL};
}

double NumberSTD::cbrt_ptr(LSNumber* x) {
	double s = cbrt(x->value);
	LSValue::delete_temporary(x);
	return s;
}

Compiler::value NumberSTD::cbrt_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::REAL, args, +[] (double x) {
		return cbrt(x);
	});
}

Compiler::value NumberSTD::pow_int(Compiler& c, std::vector<Compiler::value> args) {
	return {jit_insn_pow(c.F, args[0].v, args[1].v), Type::INTEGER};
}

Compiler::value NumberSTD::is_prime(Compiler& c, std::vector<Compiler::value> args) {
	auto v_addr = c.insn_address_of(args[0]);
	auto reps = c.new_integer(15);
	return c.insn_call(Type::INTEGER, {v_addr, reps}, &mpz_probab_prime_p);
}
Compiler::value NumberSTD::is_prime_tmp(Compiler& c, std::vector<Compiler::value> args) {
	auto v_addr = c.insn_address_of(args[0]);
	auto reps = c.new_integer(15);
	auto res = c.insn_call(Type::INTEGER, {v_addr, reps}, &mpz_probab_prime_p);
	VM::delete_gmp_int(c.F, args[0].v);
	return res;
}
template<typename T>
int NumberSTD::is_prime_number(T n) {
	if (((!(n & 1)) and n != 2) or (n < 2) || (n % 3 == 0 and n != 3)) {
		return false;
	}
	for (T k = 1; 36 * k * k - 12 * k < n; ++k) {
		if ((n % (6 * k + 1) == 0) or (n % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
}
Compiler::value NumberSTD::is_prime_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args, &NumberSTD::is_prime_number<int>);
}
Compiler::value NumberSTD::is_prime_long(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::BOOLEAN, args, &NumberSTD::is_prime_number<long>);
}

}
