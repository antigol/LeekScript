#include <sstream>
#include <algorithm>
#include "LSString.hpp"
#include "LSNull.hpp"
#include "LSBoolean.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"
#include <string.h>
#include "../../../lib/utf8.h"

using namespace std;

namespace ls {

LSValue* LSString::string_class(new LSClass("String"));

LSString::LSString() {}
LSString::LSString(const char value) : string(string(1, value)) {}
LSString::LSString(const char* value) : string(value) {}
LSString::LSString(const string& value) : string(value) {}
LSString::LSString(const Json& json) : string(json.get<std::string>()) {}

LSString::~LSString() {}

LSString* LSString::charAt(int index) const {
	return new LSString(this->operator[] (index));
}

int LSString::unicode_length() const {
	return u8_strlen(this->c_str());
}

/*
 * LSValue methods
 */
bool LSString::isTrue() const {
	return size() > 0;
}

LSValue* LSString::ls_not() {
	bool r = size() == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

LSValue* LSString::ls_tilde() {
	char buff[5];
	char* string_chars = (char*) this->c_str();
	int i = 0;
	int l = strlen(string_chars);
	string reversed = "";
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		reversed = buff + reversed;
	}
	if (refs == 0) delete this;
	return new LSString(reversed);
}

LSValue* LSString::ls_add(LSNull*) {
	if (refs == 0) {
		this->append("null");
		return this;
	}
	return new LSString(*this + "null");
}
LSValue* LSString::ls_add(LSBoolean* boolean) {
	if (refs == 0) {
		this->append(boolean->value ? "true" : "false");
		return this;
	}
	return new LSString(*this + (boolean->value ? "true" : "false"));
}
LSValue* LSString::ls_add(LSNumber* value) {
	if (refs == 0) {
		this->append(value->toString());
		if (value->refs == 0) delete value;
		return this;
	}
	LSValue* r = new LSString(*this + value->toString());
	if (value->refs == 0) delete value;
	return r;
}
LSValue* LSString::ls_add(LSString* string) {
	if (refs == 0) {
		this->append(*string);
		if (string->refs == 0) delete string;
		return this;
	}
	LSValue* r = new LSString(*this + *string);
	if (string->refs == 0) delete string;
	return r;
}
LSValue* LSString::ls_add(LSArray<LSValue*>* array) {
	if (refs == 0) {
		this->append("<array>");
		if (array->refs == 0) delete array;
		return this;
	}
	LSValue* r = new LSString(*this + "<array>");
	if (array->refs == 0) delete array;
	return r;
}
LSValue* LSString::ls_add(LSArray<int>* array) {
	if (refs == 0) {
		this->append("<array>");
		if (array->refs == 0) delete array;
		return this;
	}
	LSValue* r = new LSString(*this + "<array>");
	if (array->refs == 0) delete array;
	return r;
}
LSValue* LSString::ls_add(LSObject* object) {
	if (refs == 0) {
		this->append("<object>");
		if (object->refs == 0) delete object;
		return this;
	}
	LSValue* r = new LSString(*this + "<object>");
	if (object->refs == 0) delete object;
	return r;
}
LSValue* LSString::ls_add(LSFunction* function) {
	if (refs == 0) {
		this->append("<function>");
		if (function->refs == 0) delete function;
		return this;
	}
	LSValue* r = new LSString(*this + "<function>");
	if (function->refs == 0) delete function;
	return r;
}
LSValue* LSString::ls_add(LSClass*) {
	if (refs == 0) {
		this->append("<class>");
		return this;
	}
	LSValue* r = new LSString(*this + "<class>");
	return r;
}

LSValue* LSString::ls_add_eq(LSNull*) {
	append("null");
	return this;
}
LSValue* LSString::ls_add_eq(LSBoolean* boolean) {
	append(boolean->value ? "true" : "false");
	return this;
}
LSValue* LSString::ls_add_eq(LSNumber* value) {
	append(value->toString());
	if (value->refs == 0) delete value;
	return this;
}
LSValue* LSString::ls_add_eq(LSString* string) {
	append(*string);
	if (string->refs == 0) delete string;
	return this;
}
LSValue* LSString::ls_add_eq(LSArray<LSValue*>* array) {
	append("<array>");
	if (array->refs == 0) delete array;
	return this;
}
LSValue* LSString::ls_add_eq(LSArray<int>* array) {
	append("<array>");
	if (array->refs == 0) delete array;
	return this;
}
LSValue* LSString::ls_add_eq(LSObject* object) {
	append("<object>");
	if (object->refs == 0) delete object;
	return this;
}
LSValue* LSString::ls_add_eq(LSFunction* function) {
	append("<function>");
	LSValue::delete_temporary(function);
	return this;
}
LSValue* LSString::ls_add_eq(LSClass*) {
	append("<class>");
	return this;
}

LSValue* LSString::ls_mul(LSNumber* number) {
	string r;
	for (int i = 0; i < number->value; ++i) {
		r += *this;
	}
	if (number->refs == 0) delete number;
	if (refs == 0) {
		*this = r;
		return this;
	}
	return new LSString(r);
}

LSValue* LSString::ls_div(LSString* s) {

	char buff[5];
	char* string_chars = (char*) this->c_str();

	LSArray<LSValue*>* array = new LSArray<LSValue*>();

	if (s->size() == 0) {
		int i = 0;
		int l = strlen(string_chars);
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			u8_toutf8(buff, 5, &c, 1);
			LSString* ch = new LSString(buff);
			array->push_inc(ch);
		}
 	} else {

 		u_int32_t separator = u8_char_at((char*) s->c_str(), 0);
		int i = 0;
		int l = strlen(string_chars);
		std::string item = "";
		while (i < l) {
			u_int32_t c = u8_nextchar(string_chars, &i);
			if (c == separator) {
				array->push_inc(new LSString(item));
				item = "";
			} else {
				u8_toutf8(buff, 5, &c, 1);
				item += buff;
			}
		}
		array->push_inc(new LSString(item));
//		stringstream ss(*this);
//		string item;
//		while (getline(ss, item, s->operator[] (0))) {
//			array->push_no_clone(new LSString(item));
//		}
 	}
	if (s->refs == 0) delete s;
	if (refs == 0) delete this;
	return array;
}

bool LSString::eq(const LSString* str) const {
	return compare(*str) == 0;
}

bool LSString::lt(const LSString* str) const {
	return compare(*str) < 0;
}


u_int32_t LSString::u8_char_at(char* s, int pos) {
	int i = 0;
	int j = 0;
	int l = strlen(s);
	while (i < l) {
		u_int32_t c = u8_nextchar(s, &i);
		if (j++ == pos) return c;
	}
	return 0;
}

LSValue* LSString::at(const LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
//		return new LSString(this->operator[] ((int) n->value));
		char buff[5];
		u_int32_t c = u8_char_at((char*) this->c_str(), (int) n->value);
		u8_toutf8(buff, 5, &c, 1);
		return new LSString(buff);
	}
	return LSNull::get();
}

LSValue** LSString::atL(const LSValue*) {
	// TODO
	return nullptr;
}

LSValue* LSString::range(int start, int end) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;

	int i = 0;
	int j = 0;
	int l = strlen(string_chars);
	while (i < l) {
		j++;
		u_int32_t c = u8_nextchar(string_chars, &i);
		if (j <= start) continue;
		u8_toutf8(buff, 5, &c, 1);
		new_string += buff;
		if (j > end) break;
	}
	return new LSString(new_string);
//	return new LSString(this->substr(start, end - start + 1));
}
LSValue* LSString::rangeL(int, int) {
	// TODO
	return this;
}

LSValue* LSString::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}

LSValue** LSString::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSString::abso() const {
	return LSNumber::get(unicode_length());
}

std::ostream& LSString::print(std::ostream& os) const {
	os << "'" << escaped('\'') << "'";
	return os;
}
string LSString::json() const {
	return "\"" + escaped('"') + "\"";
}

string LSString::escaped(char quote) const {

	char buff[5];
	char* string_chars = (char*) this->c_str();
	std::string new_string;

	int i = 0;
	int l = strlen(string_chars);
	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		if (c == '\b') {
			new_string += "\\b";
		} else if (c == '\f') {
			new_string += "\\f";
		} else if (c == '\n') {
			new_string += "\\n";
		} else if (c == '\r') {
			new_string += "\\r";
		} else if (c == '\t') {
			new_string += "\\t";
		} else if (c == (u_int32_t)quote) {
			new_string += '\\';
			new_string += quote;
		} else {
			u8_toutf8(buff, 5, &c, 1);
			new_string += buff;
		}
	}
	return new_string;
}

LSValue* LSString::clone() const {
	return new LSString((std::string) *this);
}

std::ostream& operator << (std::ostream& os, const LSString& obj) {
	os << obj;
	return os;
}

LSValue* LSString::getClass() const {
	return LSString::string_class;
}

const BaseRawType* LSString::getRawType() const {
	return RawType::STRING;
}

}
