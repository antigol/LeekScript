#define NUMBER_TYPE double

#ifndef LSNUMBER_H_
#define LSNUMBER_H_

#include <iostream>
#include <gmp.h>
#include <gmpxx.h>

#include "../LSValue.hpp"
#include "LSString.hpp"
#include "LSClass.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

#define USE_CACHE 0
#define CACHE_LOW -128
#define CACHE_HIGH 1000

namespace ls {

class LSNumber : public LSValue {
public:

#if USE_CACHE
	const NUMBER_TYPE value;
#else
	NUMBER_TYPE value;
#endif

	static LSClass* number_class;
	static LSNumber* cache[CACHE_HIGH - CACHE_LOW + 1];
	static void build_cache();
	static LSNumber* get(NUMBER_TYPE);
	static std::string print(double);

	LSNumber();
	LSNumber(NUMBER_TYPE value);
	LSNumber(Json& data);

	virtual ~LSNumber();

	int integer() const;

	/*
	 * LSValue
	 */
	bool isTrue() const override;

	LSValue* ls_minus() override;
	LSValue* ls_not() override;
	LSValue* ls_tilde() override;
	LSValue* ls_preinc() override;
	LSValue* ls_inc() override;
	LSValue* ls_predec() override;
	LSValue* ls_dec() override;

	LSVALUE_OPERATORS

	LSValue* ls_add(LSNull*) override;
	LSValue* ls_add(LSBoolean*) override;
	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString*) override;

	LSValue* ls_add_eq(LSNull*) override;
	LSValue* ls_add_eq(LSBoolean*) override;
	LSValue* ls_add_eq(LSNumber*) override;

	LSValue* ls_sub(LSNull*) override;
	LSValue* ls_sub(LSBoolean*) override;
	LSValue* ls_sub(LSNumber*) override;

	LSValue* ls_sub_eq(LSNull*) override;
	LSValue* ls_sub_eq(LSBoolean*) override;
	LSValue* ls_sub_eq(LSNumber*) override;

	LSValue* ls_mul(LSNull*) override;
	LSValue* ls_mul(LSBoolean*) override;
	LSValue* ls_mul(LSNumber*) override;
	LSValue* ls_mul(LSString*) override;

	LSValue* ls_mul_eq(LSNull*) override;
	LSValue* ls_mul_eq(LSBoolean*) override;
	LSValue* ls_mul_eq(LSNumber*) override;

	LSValue* ls_div(LSNull*) override;
	LSValue* ls_div(LSBoolean*) override;
	LSValue* ls_div(LSNumber*) override;

	LSValue* ls_int_div(LSNumber*) override;

	LSValue* ls_div_eq(LSNull*) override;
	LSValue* ls_div_eq(LSBoolean*) override;
	LSValue* ls_div_eq(LSNumber*) override;

	LSValue* ls_pow(LSNull*) override;
	LSValue* ls_pow(LSBoolean*) override;
	LSValue* ls_pow(LSNumber*) override;

	LSValue* ls_pow_eq(LSNull*) override;
	LSValue* ls_pow_eq(LSBoolean*) override;
	LSValue* ls_pow_eq(LSNumber*) override;

	LSValue* ls_mod(LSNull*) override;
	LSValue* ls_mod(LSBoolean*) override;
	LSValue* ls_mod(LSNumber*) override;

	LSValue* ls_mod_eq(LSNull*) override;
	LSValue* ls_mod_eq(LSBoolean*) override;
	LSValue* ls_mod_eq(LSNumber*) override;

	bool eq(const LSNumber*) const override;
	bool lt(const LSNumber*) const override;

	LSValue* at(const LSValue* value) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;
	std::string toString() const;

	LSValue* getClass() const override;

	bool isInteger() const;

	int typeID() const override { return 3; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
