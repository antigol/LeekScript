#include "Value.hpp"
#include "../../vm/Type.hpp"

namespace ls {

Value::Value() {
	constant = false;

	// important initialisation
	type = Type::UNKNOWN;
	analysed = false;
}

Value::~Value() {}

bool Value::isLeftValue() const {
	return false;
}

void Value::add_error(SemanticAnalyser* analyser, SemanticException::Type error_type)
{
	std::stringstream oss;
	print(oss);
	analyser->add_error({ error_type, line(), oss.str() });
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

}
