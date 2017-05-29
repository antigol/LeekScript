#include <cmath>
#include <sstream>

#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSBoolean.hpp"

using namespace std;

namespace ls {

LSClass* LSNumber::clazz;

LSNumber* LSNumber::get(NUMBER_TYPE i) {
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

LSNumber::LSNumber(NUMBER_TYPE value) : LSValue(NUMBER), value(value) {}

LSNumber::~LSNumber() {}

/*
 * LSNumber methods
 */
LSValue* LSNumber::ls_fold(LSFunction<LSValue*>* function, LSValue* v0) {
	auto fun = (LSValue* (*)(void*, LSValue*, int)) function->function;
	LSValue* result = ls::move(v0);
	int number = this->value;
	while (number) {
        int digit = number % 10;
		result = fun(function, result, digit);
        number /= 10;
    }
	LSValue::delete_temporary(this);
	return result;
}

/*
 * LSValue methods
 */
bool LSNumber::to_bool() const {
	return value != 0;
}

bool LSNumber::ls_not() const {
	bool r = value == 0;
	LSValue::delete_temporary(this);
	return r;
}

LSValue* LSNumber::ls_minus() {
	if (refs == 0) {
		value = -value;
		return this;
	}
	return LSNumber::get(-value);
}

LSValue* LSNumber::ls_tilde() {
	if (refs == 0) {
		value = ~ (int)value;
		return this;
	}
	return LSNumber::get(~ (int)value);
}

LSValue* LSNumber::ls_preinc() {
	value += 1;
	return this;
}

LSValue* LSNumber::ls_inc() {
	LSValue* r = LSNumber::get(value);
	value += 1;
	return r;
}

LSValue* LSNumber::ls_predec() {
	value -= 1;
	return this;
}

LSValue* LSNumber::ls_dec() {
	LSValue* r = LSNumber::get(value);
	value -= 1;
	return r;
}

LSValue* LSNumber::add(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			value += number->value;
			LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			number->value += value;
			return number;
		}
		return LSNumber::get(this->value + number->value);
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		if (boolean->value) {
			if (refs == 0) {
				this->value += 1;
				return this;
			}
			return LSNumber::get(value + 1);
		}
		return this;
	}
	if (v->type != STRING) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto string = static_cast<LSString*>(v);
	LSValue* r = new LSString(toString() + *string);
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(string);
	return r;
}

LSValue* LSNumber::add_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value += number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	value += boolean->value;
	return this;
}

LSValue* LSNumber::sub(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			value -= number->value;
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = this->value - number->value;
			return number;
		}
		return LSNumber::get(this->value - number->value);
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		if (refs == 0) {
			this->value -= 1;
			return this;
		}
		return LSNumber::get(value - 1);
	}
	return this;
}

LSValue* LSNumber::sub_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value -= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	value -= boolean->value;
	return this;
}

LSValue* LSNumber::mul(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
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
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		if (boolean->value) {
			return this;
		}
		if (refs == 0) {
			value = 0;
			return this;
		}
		return LSNumber::get(0);
	}
	if (v->type != STRING) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto string = static_cast<LSString*>(v);
	std::string r;
	for (int i = 0; i < value; ++i) {
		r += *string;
	}
	if (refs == 0) delete this;
	if (string->refs == 0) {
		*string = r;
		return string;
	}
	return new LSString(r);
}

LSValue* LSNumber::mul_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value *= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	value *= boolean->value;
	return this;
}

LSValue* LSNumber::div(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
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
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = NAN;
		return this;
	}
	return LSNumber::get(NAN);
}

LSValue* LSNumber::div_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value /= number->value;
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		value = NAN;
	}
	return this;
}

LSValue* LSNumber::int_div(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			value /= number->value;
			value = floor(value);
			LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			number->value = floor(value / number->value);
			return number;
		}
		return LSNumber::get(floor(value / number->value));
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::DIVISION_BY_ZERO));
	}
	if (refs == 0) {
		value = floor(value);
		return this;
	}
	return LSNumber::get(floor(value));
}

LSValue* LSNumber::int_div_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value /= number->value;
		value = floor(value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::DIVISION_BY_ZERO));
	}
	value = floor(value);
	return this;
}

LSValue* LSNumber::pow(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			value = std::pow(value, number->value);
			if (number->refs == 0) delete number;
			return this;
		}
		if (number->refs == 0) {
			number->value = std::pow(value, number->value);
			return number;
		}
		return LSNumber::get(std::pow(value, number->value));
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = 1;
		return this;
	}
	return LSNumber::get(1);
}

LSValue* LSNumber::pow_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value = std::pow(value, number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	auto boolean = static_cast<LSBoolean*>(v);
	if (!boolean->value) value = 1;
	return this;
}

LSValue* LSNumber::mod(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (refs == 0) {
			value = fmod(value, number->value);
			LSValue::delete_temporary(number);
			return this;
		}
		if (number->refs == 0) {
			number->value = fmod(value, number->value);
			return number;
		}
		return LSNumber::get(fmod(value, number->value));
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(this);
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}

LSValue* LSNumber::mod_eq(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		value = fmod(value, number->value);
		LSValue::delete_temporary(number);
		return this;
	}
	if (v->type != BOOLEAN) {
		LSValue::delete_temporary(v);
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
	}
	value = 0;
	return this;
}

bool LSNumber::operator == (int v) const {
	return value == v;
}

bool LSNumber::operator == (double v) const {
	return value == v;
}

bool LSNumber::operator < (int v) const {
	return value < v;
}

bool LSNumber::operator < (double v) const {
	return value < v;
}

bool LSNumber::eq(const LSValue* v) const {
	if (v->type == NUMBER) {
		auto number = static_cast<const LSNumber*>(v);
		return this->value == number->value;
	}
	return false;
}

bool LSNumber::lt(const LSValue* v) const {
	if (v->type == NUMBER) {
		auto number = static_cast<const LSNumber*>(v);
		return this->value < number->value;
	}
	return LSValue::lt(v);
}

int LSNumber::abso() const {
	return abs((int) value);
}

LSValue* LSNumber::clone() const {
	return LSNumber::get(this->value);
}

bool LSNumber::isInteger() const {
	return value == (int)value;
}

string LSNumber::toString() const {

	if (isInteger()) return std::to_string((int)value);

	return LSNumber::print(value);
}
string LSNumber::json() const {
	return toString();
}

std::ostream& LSNumber::dump(std::ostream& os) const {
	os << toString();
	return os;
}

LSValue* LSNumber::getClass() const {
	return LSNumber::clazz;
}

}
