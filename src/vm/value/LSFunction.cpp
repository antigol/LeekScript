#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSClass* LSFunction::function_class = new LSClass("Function");

LSFunction::LSFunction(void* function) {
	this->function = function;
	this->refs = 0;
	this->native = false;
}

LSFunction::LSFunction(void* function, int refs, bool native) {
	this->function = function;
	this->refs = refs;
	this->native = native;
}

LSFunction::LSFunction(Json&) {
	// TODO
}

LSFunction::~LSFunction() {}

bool LSFunction::isTrue() const {
	return true;
}

bool LSFunction::eq(const LSFunction* f) const {
	return function == f->function;
}

bool LSFunction::lt(const LSFunction* f) const {
	return function < f->function;
}


LSValue* LSFunction::at(const LSValue*) const {
	return LSNull::get();
}
LSValue** LSFunction::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSFunction::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSFunction::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSFunction::clone() const {
	return (LSValue*) this;
}

std::ostream& LSFunction::print(std::ostream& os) const {
	os << "<function>";
	return os;
}
string LSFunction::json() const {
	return "\"<function>\"";
}

LSValue* LSFunction::getClass() const {
	return LSFunction::function_class;
}

const BaseRawType* LSFunction::getRawType() const {
	return RawType::FUNCTION;
}

}
