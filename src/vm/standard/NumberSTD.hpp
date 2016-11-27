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
	static Compiler::value store_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value store_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value add_real_real(Compiler& c, std::vector<Compiler::value>);
	static LSValue* add_int_ptr(int a, LSValue* b);
	static Compiler::value add_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value add_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value add_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value add_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value add_eq_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value sub_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_gmp_int(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value sub_gmp_tmp_int(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mul_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mul_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value pow_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value pow_gmp_int(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value lt_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value lt_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value lt_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value lt_gmp_tmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value gt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value gt_int_gmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mod_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value mod_gmp_gmp_tmp(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value eq_gmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value eq_gmp_tmp_gmp(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value eq_gmp_int(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value eq_gmp_tmp_int(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value tilde_int(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value tilde_real(Compiler& c, std::vector<Compiler::value> args);

	/*
	 * Methods
	 */
	static double abs_ptr(LSNumber* x);
	static Compiler::value abs_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value abs_int(Compiler&, std::vector<Compiler::value>);

	static double acos_ptr(LSNumber* x);
	static Compiler::value acos_real(Compiler&, std::vector<Compiler::value>);

	static double asin_ptr(LSNumber* x);
	static Compiler::value asin_real(Compiler&, std::vector<Compiler::value>);

	static double atan_ptr(LSNumber* x);
	static Compiler::value atan_real(Compiler&, std::vector<Compiler::value>);

	static double atan2_ptr_ptr(LSNumber*, LSNumber*);
	static double atan2_ptr_real(LSNumber*, double);
	static double atan2_real_ptr(double, LSNumber*);
	static Compiler::value atan2_real_real(Compiler&, std::vector<Compiler::value>);

	static LSString* char_ptr(LSNumber* x);
	static Compiler::value char_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value char_int(Compiler&, std::vector<Compiler::value>);

	static double exp_ptr(LSNumber* x);
	static Compiler::value exp_real(Compiler&, std::vector<Compiler::value>);

	static int floor_ptr(LSNumber* x);
	static Compiler::value floor_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value floor_int(Compiler&, std::vector<Compiler::value>);

	static int round_ptr(LSNumber* x);
	static Compiler::value round_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value round_int(Compiler&, std::vector<Compiler::value>);

	static int ceil_ptr(LSNumber* x);
	static Compiler::value ceil_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value ceil_int(Compiler&, std::vector<Compiler::value>);

	static double max_ptr_ptr(LSNumber* x, LSNumber* y);
	static double max_ptr_float(LSNumber* x, double y);
	static double max_ptr_int(LSNumber* x, int y);
	static double max_float_ptr(double x, LSNumber* y);
	static Compiler::value max_float_float(Compiler&, std::vector<Compiler::value>);
	static double max_int_ptr(int x, LSNumber* y);

	static double min_ptr_ptr(LSNumber* x, LSNumber* y);
	static double min_ptr_float(LSNumber* x, double y);
	static double min_ptr_int(LSNumber* x, int y);
	static double min_float_ptr(double x, LSNumber* y);
	static Compiler::value min_float_float(Compiler&, std::vector<Compiler::value>);
	static double min_int_ptr(int x, LSNumber* y);

	static double cos_ptr(LSNumber* x);
	static Compiler::value cos_real(Compiler&, std::vector<Compiler::value>);

	static double sin_ptr(LSNumber* x);
	static Compiler::value sin_real(Compiler&, std::vector<Compiler::value>);

	static double tan_ptr(LSNumber* x);
	static Compiler::value tan_real(Compiler&, std::vector<Compiler::value>);

	static double sqrt_ptr(LSNumber* x);
	static Compiler::value sqrt_real(Compiler&, std::vector<Compiler::value>);

	static double cbrt_ptr(LSNumber* x);
	static Compiler::value cbrt_real(Compiler&, std::vector<Compiler::value>);

	static Compiler::value pow_int(Compiler&, std::vector<Compiler::value>);

	static Compiler::value is_prime(Compiler&, std::vector<Compiler::value>);
	static Compiler::value is_prime_tmp(Compiler&, std::vector<Compiler::value>);
	template<typename T>
	static int is_prime_number(T n);
	static Compiler::value is_prime_int(Compiler&, std::vector<Compiler::value>);
	static Compiler::value is_prime_long(Compiler&, std::vector<Compiler::value>);
};

}

#endif
