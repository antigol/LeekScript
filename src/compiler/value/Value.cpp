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

void Value::preanalyse(SemanticAnalyser* analyser)
{
	analyse(analyser, Type::UNKNOWN);
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

}
