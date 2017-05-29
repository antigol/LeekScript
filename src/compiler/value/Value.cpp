#include <sstream>
#include "Value.hpp"
#include "../../vm/Type.hpp"

namespace ls {

Value::Value() {
	type = Type::UNKNOWN;
	constant = false;
}

Value::~Value() {}

bool Value::will_take(SemanticAnalyser*, const std::vector<Type>& args, int) {
	auto r = type.will_take(args);
	set_version(args);
	return r;
}

bool Value::will_store(SemanticAnalyser*, const Type&) {
	return false;
}

bool Value::must_be_pointer(SemanticAnalyser*) {
	if (type.nature == Nature::POINTER) {
		return false;
	}
	type.nature = Nature::POINTER;
	types = type;
	return true;
}

void Value::must_return(SemanticAnalyser*, const Type& ret_type) {
	type.setReturnType(ret_type);
}

void Value::will_be_in_array(SemanticAnalyser*) {}

void Value::set_version(std::vector<Type> args) {
	version = args;
	has_version = true;
}

Type Value::version_type(std::vector<Type>) const {
	return type;
}

bool Value::isLeftValue() const {
	return false;
}

Compiler::value Value::compile_version(Compiler& c, std::vector<Type> args) const {
	return compile(c);
}

std::string Value::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

std::string Value::to_string() const {
	std::ostringstream oss;
	oss << this;
	return oss.str();
}

std::ostream& operator << (std::ostream& os, const Value* v) {
	v->print(os);
	return os;
}

}
