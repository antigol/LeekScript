#include <sstream>
#include <algorithm>
#include "LSString.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

LSValue* LSString::string_class(new LSClass("String"));

LSString::LSString() {}
LSString::LSString(const char value) : string(string(1, value)) {}
LSString::LSString(const char* value) : string(value) {}
LSString::LSString(std::string value) : string(value) {}
LSString::LSString(JsonValue& json) : string(json.toString()) {}

LSString::~LSString() {}


LSString* LSString::charAt(int index) const {
	return new LSString(this->operator[] (index));
}


/*
 * LSValue methods
 */
bool LSString::isTrue() const {
	return size() > 0;
}

LSValue* LSString::operator - () const {
	return this->clone();
}

LSValue* LSString::operator ! () const {
	return LSBoolean::get(size() == 0);
}

LSValue* LSString::operator ~ () const {
	string copy = *this;
	reverse(copy.begin(), copy.end());
	return new LSString(copy);
}

LSValue* LSString::operator ++ () {
	return this;
}
LSValue* LSString::operator ++ (int) {
	return this;
}

LSValue* LSString::operator -- () {
	return this;
}
LSValue* LSString::operator -- (int) {
	return this;
}

LSValue* LSString::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSString::operator + (const LSNull*) const {
	return new LSString(*this + "null");
}
LSValue* LSString::operator + (const LSBoolean* boolean) const {
	return new LSString(*this + (boolean->value ? "true" : "false"));
}
LSValue* LSString::operator + (const LSNumber* value) const {
	return new LSString(*this + value->toString());
}
LSValue* LSString::operator + (const LSString* string) const {
	return new LSString(*this + *string);
}
LSValue* LSString::operator + (const LSArray<LSValue*>*) const {
	return new LSString(*this + "<array>");
}
LSValue* LSString::operator + (const LSArray<int>*) const {
	return new LSString(*this + "<array>");
}
LSValue* LSString::operator + (const LSObject* ) const {
	return new LSString(*this + "<object>");
}
LSValue* LSString::operator + (const LSFunction*) const {
	return new LSString(*this + "<function>");
}
LSValue* LSString::operator + (const LSClass*) const {
	return new LSString(*this + "<class>");
}

LSValue* LSString::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSString::operator += (const LSNull*) {
	return this;
}
LSValue* LSString::operator += (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSNumber*) {
	return this;
}
LSValue* LSString::operator += (const LSString* string) {
	((std::string*) this)->operator += (*string);
	return this;
}
LSValue* LSString::operator += (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator += (const LSClass*) {
	return this;
}

LSValue* LSString::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSString::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSNumber* value) const {
	return new LSString(to_string(value->value) + *this);
}
LSValue* LSString::operator - (const LSString* string) const {
	return new LSString(*string + *this);
}
LSValue* LSString::operator - (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSString::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSString::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSString::operator -= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator -= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSString::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSNumber* value) const {
	string res = "";
	for (int i = 0; i < value->value; ++i) {
		res += *this;
	}
	return new LSString(res);
}
LSValue* LSString::operator * (const LSString* string) const {
	return new LSString(*string);
}
LSValue* LSString::operator * (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSString::operator *= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator *= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSString::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSNumber*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator / (const LSString* s) const {
	LSArray<LSValue*>* array = new LSArray<LSValue*>();
	if (s->size() == 0) {
		for (char c : *this) {
			array->push_no_clone(new LSString(string({c})));
		}
 	} else {
		stringstream ss(*this);
		string item;
		while (getline(ss, item, s->operator[] (0))) {
			array->push_no_clone(new LSString(item));
		}
 	}
	return array;
}

LSValue* LSString::operator / (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSString::operator /= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator /= (const LSClass*) {
	return this->clone();
}

LSValue* LSString::poww(const LSValue*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::pow_eq(LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSString::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSString::pow_eq(const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSNumber*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSString*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSObject*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSFunction*) {
	return this->clone();
}
LSValue* LSString::pow_eq(const LSClass*) {
	return this->clone();
}

LSValue* LSString::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSString::operator % (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSString::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSString::operator %= (LSValue* value) {
	return value->operator %= (this);
}
LSValue* LSString::operator %= (const LSNull*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSBoolean*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSNumber*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSString*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSArray<LSValue*>*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSObject*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSFunction*) {
	return this->clone();
}
LSValue* LSString::operator %= (const LSClass*) {
	return this->clone();
}

bool LSString::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSString::operator == (const LSNull*) const {
	return false;
}
bool LSString::operator == (const LSBoolean*) const {
	return false;
}
bool LSString::operator == (const LSNumber*) const {
	return false;
}
bool LSString::operator == (const LSString* v) const {
	return *this == *v;
}
bool LSString::operator == (const LSArray<LSValue*>*) const {
	return false;
}
bool LSString::operator == (const LSFunction*) const {
	return false;
}
bool LSString::operator == (const LSObject*) const {
	return false;
}
bool LSString::operator == (const LSClass*) const {
	return false;
}

bool LSString::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSString::operator < (const LSNull*) const {
	return false;
}
bool LSString::operator < (const LSBoolean*) const {
	return false;
}
bool LSString::operator < (const LSNumber*) const {
	return false;
}
bool LSString::operator < (const LSString* v) const {
	return *this < *v;
}
bool LSString::operator < (const LSArray<LSValue*>*) const {
	return true;
}
bool LSString::operator < (const LSObject*) const {
	return true;
}
bool LSString::operator < (const LSFunction*) const {
	return true;
}
bool LSString::operator < (const LSClass*) const {
	return true;
}

bool LSString::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSString::operator > (const LSNull*) const {
	return true;
}
bool LSString::operator > (const LSBoolean*) const {
	return true;
}
bool LSString::operator > (const LSNumber*) const {
	return true;
}
bool LSString::operator > (const LSString* v) const {
	return *this > *v;
}
bool LSString::operator > (const LSArray<LSValue*>*) const {
	return false;
}
bool LSString::operator > (const LSObject*) const {
	return false;
}
bool LSString::operator > (const LSFunction*) const {
	return false;
}
bool LSString::operator > (const LSClass*) const {
	return false;
}

bool LSString::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSString::operator <= (const LSNull*) const {
	return false;
}
bool LSString::operator <= (const LSBoolean*) const {
	return false;
}
bool LSString::operator <= (const LSNumber*) const {
	return false;
}
bool LSString::operator <= (const LSString* v) const {
	return *this <= *v;
}
bool LSString::operator <= (const LSArray<LSValue*>*) const {
	return true;
}
bool LSString::operator <= (const LSObject*) const {
	return true;
}
bool LSString::operator <= (const LSFunction*) const {
	return true;
}
bool LSString::operator <= (const LSClass*) const {
	return true;
}

bool LSString::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSString::operator >= (const LSNull*) const {
	return true;
}
bool LSString::operator >= (const LSBoolean*) const {
	return true;
}
bool LSString::operator >= (const LSNumber*) const {
	return true;
}
bool LSString::operator >= (const LSString* v) const {
	return *this >= *v;
}
bool LSString::operator >= (const LSArray<LSValue*>*) const {
	return false;
}
bool LSString::operator >= (const LSObject*) const {
	return false;
}
bool LSString::operator >= (const LSFunction*) const {
	return false;
}
bool LSString::operator >= (const LSClass*) const {
	return false;
}

bool LSString::in(const LSValue*) const {
	return false;
}

LSValue* LSString::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		return new LSString(this->operator[] ((int) n->value));
	}
	return LSNull::null_var;
}

LSValue** LSString::atL(const LSValue*) {
	// TODO
	return &LSNull::null_var;
}

LSValue* LSString::range(int start, int end) const {
	return new LSString(this->substr(start, end - start + 1));
}
LSValue* LSString::rangeL(int, int) {
	// TODO
	return this;
}

LSValue* LSString::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::null_var;
}

LSValue** LSString::attrL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSString::abso() const {
	return LSNumber::get(size());
}

std::ostream& LSString::print(std::ostream& os) const {
	os << "'" << *this << "'";
	return os;
}
string LSString::json() const {
	return "\"" + *this + "\"";
}

LSValue* LSString::clone() const {
	return new LSString(*this);
}

std::ostream& operator << (std::ostream& os, const LSString& obj) {
	os << obj;
	return os;
}

LSValue* LSString::getClass() const {
	return LSString::string_class;
}

int LSString::typeID() const {
	return 4;
}

const BaseRawType* LSString::getRawType() const {
	return RawType::STRING;
}
