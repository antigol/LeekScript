#include <cmath>
#include <sstream>

#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSBoolean.hpp"

using namespace std;

namespace ls {

LSClass* LSNumber::number_class = new LSClass("Number");

LSNumber* LSNumber::cache[CACHE_HIGH - CACHE_LOW + 1];

void LSNumber::build_cache() {
	for (int i = CACHE_LOW; i <= CACHE_HIGH; ++i) {
		cache[-CACHE_LOW + i] = new LSNumber(i);
	}
}

LSNumber* LSNumber::get(NUMBER_TYPE i) {
#if USE_CACHE
	if ((i == (int) i) and i >= CACHE_LOW and i <= CACHE_HIGH) {
		return cache[(int) (-CACHE_LOW + i)];
	}
#endif
//	std::cout << "Number() " << i << endl;
	return new LSNumber(i);
}

std::string LSNumber::print(double d) {
	std::string s;
	size_t len = snprintf(0, 0, "%.10f", d);
	size_t oldsize = s.size();
	s.resize(oldsize + len + 1);

	// technically non-portable
	snprintf(&s[oldsize], len+1, "%.10f", d);
	// remove nul terminator
	s.pop_back();
	// remove trailing zeros
	s.erase(s.find_last_not_of('0') + 1, string::npos);
	// remove trailing point
	if (s.back() == L'.') {
		s.pop_back();
	}
	return s;
}

LSNumber::LSNumber() : value(0) {}

LSNumber::LSNumber(NUMBER_TYPE value) : value(value) {}

LSNumber::LSNumber(Json& json) : value(json) {}

LSNumber::~LSNumber() {
//	cout << "delete LSNumber : " << value << endl;
}

int LSNumber::integer() const {
//	return mpz_get_ui(value.get_mpz_t());
	return 0;
}

bool LSNumber::isTrue() const {
	return value != 0;
}

LSValue*LSNumber::ls_minus() {
	if (refs == 0) {
		value = -value;
		return this;
	}
	return LSNumber::get(-value);
}

LSValue*LSNumber::ls_not() {
	bool r = value == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

LSValue*LSNumber::ls_tilde() {

	if (refs == 0) {
		value = ~ (int)value;
		return this;
	}
	return LSNumber::get(~ (int)value);
}

LSValue*LSNumber::ls_preinc() {
	// ++x
	value += 1;
	return this;
}

LSValue* LSNumber::ls_inc() {
	// x++
	if (refs == 0) {
		value += 1;
		return this;
	}
	LSValue* r = LSNumber::get(value);
	value += 1;
	return r;
}

LSValue* LSNumber::ls_predec() {
	value -= 1;
	return this;
}

LSValue* LSNumber::ls_dec() {
	if (refs == 0) {
		value -= 1;
		return this;
	}
	LSValue* r = LSNumber::get(value);
	value -= 1;
	return r;
}

LSValue* LSNumber::ls_add(LSNull*) {
	LSValue::delete_temporary(this);
	return LSNull::get();
}
LSValue* LSNumber::ls_add(LSBoolean* boolean) {
	if (boolean->value) {
		if (refs == 0) {
			this->value += 1;
			return this;
		}
		return LSNumber::get(value + 1);
	}
	return this;
}
LSValue* LSNumber::ls_add(LSString* string) {
	LSValue* r = new LSString(toString() + *string);
	if (refs == 0) delete this;
	if (string->refs == 0) delete string;
	return r;
}
LSValue* LSNumber::ls_add(LSNumber* number) {
	if (refs == 0) {
		value += number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value += value;
		return number;
	}
	return LSNumber::get(this->value + number->value);
}

LSValue* LSNumber::ls_add_eq(LSNull*) {
	return LSNull::get();
}
LSValue* LSNumber::ls_add_eq(LSBoolean* boolean) {
	value += boolean->value;
	return this;
}
LSValue* LSNumber::ls_add_eq(LSNumber* number) {
	value += number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_sub(LSNull*) {
	return LSNull::get();
}
LSValue* LSNumber::ls_sub(LSBoolean* boolean) {
	if (boolean->value) {
		if (refs == 0) {
			this->value -= 1;
			return this;
		}
		return LSNumber::get(value - 1);
	}
	return this;
}
LSValue* LSNumber::ls_sub(LSNumber* number) {
	if (refs == 0) {
		value -= number->value;
		return this;
	}
	if (number->refs == 0) {
		number->value = this->value - number->value;
		return number;
	}
	return LSNumber::get(this->value - number->value);
}

LSValue* LSNumber::ls_sub_eq(LSNull*) {
	return this;
}
LSValue* LSNumber::ls_sub_eq(LSBoolean* boolean) {
	value -= boolean->value;
	return this;
}
LSValue* LSNumber::ls_sub_eq(LSNumber* number) {
	value -= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_mul(LSNull*) {
	LSValue::delete_temporary(this);
	return LSNull::get();
}
LSValue* LSNumber::ls_mul(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mul(LSNumber* number) {
	if (refs == 0) {
		value *= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value *= value;
		return number;
	}
	return LSNumber::get(value * number->value);
}

LSValue*LSNumber::ls_mul(LSString* str) {
	string r;
	for (int i = 0; i < value; ++i) {
		r += *str;
	}
	if (refs == 0) delete this;

	if (str->refs == 0) {
		*str = r;
		return str;
	}
	return new LSString(r);
}

LSValue* LSNumber::ls_mul_eq(LSNull*) {
	return LSNull::get();
}
LSValue* LSNumber::ls_mul_eq(LSBoolean* boolean) {
	value *= boolean->value;
	return this;
}
LSValue* LSNumber::ls_mul_eq(LSNumber* number) {
	value *= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_div(LSNull*) {
	LSValue::delete_temporary(this);
	return LSNull::get();
}
LSValue* LSNumber::ls_div(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = NAN;
		return this;
	}
	return LSNumber::get(NAN);
}

LSValue* LSNumber::ls_div(LSNumber* number) {
	if (refs == 0) {
		value /= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = value / number->value;
		return number;
	}
	return LSNumber::get(value / number->value);
}

LSValue* LSNumber::ls_int_div(LSNumber* number) {
	if (refs == 0) {
		value /= number->value;
		value = floor(value);
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = floor(value / number->value);
		return number;
	}
	return LSNumber::get(floor(value / number->value));
}

LSValue* LSNumber::ls_div_eq(LSNull*) {
	value = NAN;
	return this;
}
LSValue* LSNumber::ls_div_eq(LSBoolean* boolean) {
	if (!boolean->value) {
		value = NAN;
	}
	return this;
}
LSValue* LSNumber::ls_div_eq(LSNumber* number) {
	value /= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_pow(LSNull*) {
	if (refs == 0) {
		value = 1;
		return this;
	}
	return LSNumber::get(1);
}
LSValue* LSNumber::ls_pow(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = 1;
		return this;
	}
	return LSNumber::get(1);
}
LSValue* LSNumber::ls_pow(LSNumber* number) {
	if (refs == 0) {
		value = pow(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = pow(value, number->value);
		return number;
	}
	return LSNumber::get(pow(value, number->value));
}

LSValue* LSNumber::ls_pow_eq(LSNull*) {
	value = 1;
	return this;
}
LSValue* LSNumber::ls_pow_eq(LSBoolean* boolean) {
	if (!boolean->value) value = 1;
	return this;
}
LSValue* LSNumber::ls_pow_eq(LSNumber* number) {
	value = pow(value, number->value);
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_mod(LSNull*) {
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mod(LSBoolean*) {
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mod(LSNumber* number) {
	if (refs == 0) {
		value = fmod(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = fmod(value, number->value);
		return number;
	}
	return LSNumber::get(fmod(value, number->value));
}

LSValue* LSNumber::ls_mod_eq(LSNull*) {
	value = 0;
	return this;
}
LSValue* LSNumber::ls_mod_eq(LSBoolean*) {
	value = 0;
	return this;
}
LSValue* LSNumber::ls_mod_eq(LSNumber* number) {
	value = fmod(value, number->value);
	if (number->refs == 0) delete number;
	return this;
}

bool LSNumber::eq(const LSNumber* number) const {
	return this->value == number->value;
}

bool LSNumber::lt(const LSNumber* number) const {
	return this->value < number->value;
}

LSValue* LSNumber::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSNumber::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSNumber::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSNumber::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSNumber::abso() const {
	return LSNumber::get(abs((int) value));
}

LSValue* LSNumber::clone() const {
	return LSNumber::get(this->value);
}

bool LSNumber::isInteger() const {
	return value == (int)value;
}

string LSNumber::toString() const {

	if (isInteger()) return to_string((int)value);

	return LSNumber::print(value);
}
string LSNumber::json() const {
	return toString();
}

std::ostream& LSNumber::print(std::ostream& os) const {
	os << toString();
	return os;
}

LSValue* LSNumber::getClass() const {
	return LSNumber::number_class;
}

const BaseRawType* LSNumber::getRawType() const {
	return RawType::INTEGER;
}

}
