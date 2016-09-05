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

void Value::will_take(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
}

void Value::will_require(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
	}
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
