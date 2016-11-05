#ifndef NUMBER_STD_HPP
#define NUMBER_STD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"
#include "../../vm/value/LSString.hpp"
#include <math.h>

namespace ls {

class LSNumber;
class Compiler;

class NumberSTD : public Module {
public:

	class OverflowException : public std::exception {
		virtual const char* what() const throw() {
			return "Number is too big! (10 000 bits max)";
		}
	};

	NumberSTD();

	/*
	 * Operators
	 */
	static jit_value_t store_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t store_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t add_real_real(Compiler& c, std::vector<jit_value_t>);
	static jit_value_t add_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t add_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t add_gmp_tmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t add_gmp_tmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t add_eq_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t sub_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t sub_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t sub_gmp_tmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t sub_gmp_tmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t mul_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t mul_gmp_tmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t mul_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t mul_gmp_tmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t pow_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t pow_gmp_int(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t lt_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t lt_gmp_tmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t lt_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t lt_gmp_tmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t gt_int_gmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t mod_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t mod_gmp_gmp_tmp(Compiler& c, std::vector<jit_value_t> args);

	static jit_value_t eq_gmp_gmp(Compiler& c, std::vector<jit_value_t> args);
	static jit_value_t eq_gmp_tmp_gmp(Compiler& c, std::vector<jit_value_t> args);

	/*
	 * Methods
	 */
	static double abs_ptr(LSNumber* x);
	static jit_value_t abs_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t abs_int(Compiler&, std::vector<jit_value_t>);

	static double acos_ptr(LSNumber* x);
	static jit_value_t acos_real(Compiler&, std::vector<jit_value_t>);

	static double asin_ptr(LSNumber* x);
	static jit_value_t asin_real(Compiler&, std::vector<jit_value_t>);

	static double atan_ptr(LSNumber* x);
	static jit_value_t atan_real(Compiler&, std::vector<jit_value_t>);

	static double atan2_ptr_ptr(LSNumber*, LSNumber*);
	static double atan2_ptr_real(LSNumber*, double);
	static double atan2_real_ptr(double, LSNumber*);
	static jit_value_t atan2_real_real(Compiler&, std::vector<jit_value_t>);

	static LSString* char_ptr(LSNumber* x);
	static jit_value_t char_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t char_int(Compiler&, std::vector<jit_value_t>);

	static int floor_ptr(LSNumber* x);
	static jit_value_t floor_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t floor_int(Compiler&, std::vector<jit_value_t>);

	static int round_ptr(LSNumber* x);
	static jit_value_t round_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t round_int(Compiler&, std::vector<jit_value_t>);

	static int ceil_ptr(LSNumber* x);
	static jit_value_t ceil_real(Compiler&, std::vector<jit_value_t>);
	static jit_value_t ceil_int(Compiler&, std::vector<jit_value_t>);

	static double max_ptr_ptr(LSNumber* x, LSNumber* y);
	static double max_ptr_float(LSNumber* x, double y);
	static double max_ptr_int(LSNumber* x, int y);
	static double max_float_ptr(double x, LSNumber* y);
	static jit_value_t max_float_float(Compiler&, std::vector<jit_value_t>);
	static double max_int_ptr(int x, LSNumber* y);

	static double min_ptr_ptr(LSNumber* x, LSNumber* y);
	static double min_ptr_float(LSNumber* x, double y);
	static double min_ptr_int(LSNumber* x, int y);
	static double min_float_ptr(double x, LSNumber* y);
	static jit_value_t min_float_float(Compiler&, std::vector<jit_value_t>);
	static double min_int_ptr(int x, LSNumber* y);

	static double cos_ptr(LSNumber* x);
	static jit_value_t cos_real(Compiler&, std::vector<jit_value_t>);

	static double sin_ptr(LSNumber* x);
	static jit_value_t sin_real(Compiler&, std::vector<jit_value_t>);

	static double tan_ptr(LSNumber* x);
	static jit_value_t tan_real(Compiler&, std::vector<jit_value_t>);

	static double sqrt_ptr(LSNumber* x);
	static jit_value_t sqrt_real(Compiler&, std::vector<jit_value_t>);

	static double cbrt_ptr(LSNumber* x);
	static jit_value_t cbrt_real(Compiler&, std::vector<jit_value_t>);

	static jit_value_t pow_int(Compiler&, std::vector<jit_value_t>);

	static jit_value_t is_prime(Compiler&, std::vector<jit_value_t>);
	static jit_value_t is_prime_tmp(Compiler&, std::vector<jit_value_t>);
};

}

#endif
