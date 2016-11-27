#include "Boolean.hpp"

#include "../../vm/VM.hpp"
#include "../../vm/value/LSBoolean.hpp"

using namespace std;

namespace ls {

Boolean::Boolean(bool value) {
	this->value = value;
	type = Type::BOOLEAN;
	constant = true;
}

Boolean::~Boolean() {}

void Boolean::print(std::ostream& os, int, bool debug) const {
	os << (value ? "true" : "false");
	if (debug) {
		os << " " << type;
	}
}

unsigned Boolean::line() const {
	return 0;
}

void Boolean::analyse(SemanticAnalyser*, const Type& req_type) {
	if (req_type.nature == Nature::POINTER) {
		type.nature = Nature::POINTER;
	}
}

Compiler::value Boolean::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {
		LSBoolean* b = LSBoolean::get(value);
		return {LS_CREATE_POINTER(c.F, b), Type::BOOLEAN_P};
	} else {
		return {LS_CREATE_BOOLEAN(c.F, (int) value), Type::BOOLEAN};
	}
}

}
