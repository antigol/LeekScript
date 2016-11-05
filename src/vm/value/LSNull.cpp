#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSNull::null_var = new LSNull();
LSClass* LSNull::null_class = new LSClass("Null");

LSValue* LSNull::get() {
	return null_var;
}

LSNull::LSNull() {
	refs = 1;
	native = true;
}

LSNull::~LSNull() {}

LSValue* LSNull::clone() const {
	return LSNull::get();
}

bool LSNull::isTrue() const {
	return false;
}

bool LSNull::eq(const LSNull*) const {
	return true;
}

bool LSNull::lt(const LSNull*) const {
	return false;
}

LSValue* LSNull::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSNull::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSNull::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSNull::attrL(const LSValue*) {
	return nullptr;
}

std::ostream& LSNull::print(std::ostream& os) const {
	os << "null";
	return os;
}
string LSNull::json() const {
	return "\"1|null\"";
}

LSValue* LSNull::getClass() const {
	return LSNull::null_class;
}

const BaseRawType* LSNull::getRawType() const {
	return RawType::NULLL;
}

}
