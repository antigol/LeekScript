#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSNull::null_var;
LSClass* LSNull::clazz;

LSValue* LSNull::get() {
	return null_var;
}

LSNull::LSNull() : LSValue(LSValue::NULLL) {
	refs = 1;
	native = true;
}

LSNull::~LSNull() {}

bool LSNull::to_bool() const {
	return false;
}

bool LSNull::ls_not() const {
	return true;
}

bool LSNull::eq(const LSValue* v) const {
	return dynamic_cast<const LSNull*>(v);
}

bool LSNull::lt(const LSValue* v) const {
	if (dynamic_cast<const LSNull*>(v)) {
		return false;
	}
	return LSValue::lt(v);
}

std::ostream& LSNull::dump(std::ostream& os) const {
	os << "null";
	return os;
}

LSValue* LSNull::getClass() const {
	return LSNull::clazz;
}

}
