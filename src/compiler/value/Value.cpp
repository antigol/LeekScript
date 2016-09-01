#include "Value.hpp"
#include "../../vm/Type.hpp"

namespace ls {

Value::Value() {
	type = Type::UNKNOWN;
	constant = false;
}

Value::~Value() {}

bool Value::isLeftValue() const {
	return false;
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

}
