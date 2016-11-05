#include "LSBoolean.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSBoolean::boolean_class(new LSClass("Boolean"));
LSBoolean* LSBoolean::false_val(new LSBoolean(false));
LSBoolean* LSBoolean::true_val(new LSBoolean(true));

LSBoolean::LSBoolean() : LSBoolean(true) {}

LSBoolean::LSBoolean(bool value) : value(value) {
	refs = 1;
	native = true;
}

LSBoolean::LSBoolean(Json& json) : LSBoolean((bool) json) {}

LSBoolean::~LSBoolean() {}

bool LSBoolean::isTrue() const {
	return value;
}

LSValue*LSBoolean::ls_not() {
	return LSBoolean::get(!value);
}

LSValue*LSBoolean::ls_tilde() {
	return LSBoolean::get(!value);
}

LSValue* LSBoolean::ls_add(LSNumber* n) {
	if (this->value) {
		if (n->refs == 0) {
			n->value += 1;
			return n;
		}
		return LSNumber::get(n->value + 1);
	}
	return n;
}

LSValue* LSBoolean::ls_add(LSString* s) {
	return new LSString((value ? "true" : "false") + *s);
}

LSValue* LSBoolean::ls_sub(LSNumber* n) {
	if (this->value) {
		if (n->refs == 0) {
			n->value = 1 - n->value;
			return n;
		}
		return LSNumber::get(1 - n->value);
	}
	return n;
}

bool LSBoolean::eq(const LSBoolean* boolean) const {
	return boolean->value == this->value;
}

bool LSBoolean::lt(const LSBoolean* boolean) const {
	return this->value < boolean->value;
}

LSValue* LSBoolean::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSBoolean::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSBoolean::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSBoolean::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSBoolean::clone() const {
	return (LSValue*) this;
}

std::ostream& LSBoolean::print(std::ostream& os) const {
	os << (value ? "true" : "false");
	return os;
}

string LSBoolean::json() const {
	return value ? "true" : "false";
}

LSValue* LSBoolean::getClass() const {
	return LSBoolean::boolean_class;
}

const BaseRawType* LSBoolean::getRawType() const {
	return RawType::BOOLEAN;
}

}
