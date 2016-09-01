#include "Type.hpp"

using namespace std;

namespace ls {

RawType::RawType(const string& name, const string& classname, const string& jsonname)
	: _name(name), _classname(classname), _jsonname(jsonname)
{}

bool RawType::operator ==(const RawType& type) const {
	return _name == type._name && _classname == type._classname && _jsonname == type._jsonname;
}

const RawType RawType::UNKNOWN("?", "?", "?");
const RawType RawType::VOID("void", "?", "void");
const RawType RawType::VAR("var", "Variable", "variable");
const RawType RawType::BOOLEAN("bool", "Boolean", "boolean");
const RawType RawType::I32("i32", "Number", "number");
const RawType RawType::I64("i64", "Number", "number");
const RawType RawType::F32("f32", "Number", "number");
const RawType RawType::F64("f64", "Number", "number");
const RawType RawType::VEC("vec", "Vec", "vec");
const RawType RawType::MAP("map", "Map", "map");
const RawType RawType::SET("set", "Set", "set");
const RawType RawType::FUNCTION("function", "?", "function");
const RawType RawType::TUPLE("tuple", "Tuple", "tuple");

const Type Type::UNKNOWN(RawType::UNKNOWN, Nature::UNKNOWN);
const Type Type::VOID(RawType::VOID, Nature::VOID);

const Type Type::VAR(RawType::VAR, Nature::LSVALUE);
const Type Type::BOOLEAN(RawType::BOOLEAN, Nature::VALUE);
const Type Type::I32(RawType::I32, Nature::VALUE);
const Type Type::I64(RawType::I64, Nature::VALUE);
const Type Type::F32(RawType::F32, Nature::VALUE);
const Type Type::F64(RawType::F64, Nature::VALUE);
const Type Type::VEC(RawType::VEC, Nature::LSVALUE, { Type::UNKNOWN });
const Type Type::MAP(RawType::VEC, Nature::LSVALUE, { Type::UNKNOWN, Type::UNKNOWN });
const Type Type::SET(RawType::VEC, Nature::LSVALUE, { Type::UNKNOWN });
const Type Type::FUNCTION(RawType::FUNCTION, Nature::VALUE);
const Type Type::TUPLE(RawType::TUPLE, Nature::VALUE);

Type::Type() :
	raw_type(RawType::UNKNOWN), nature(Nature::UNKNOWN)
{}

Type::Type(const RawType& raw_type, Nature nature) :
	raw_type(raw_type), nature(nature)
{
	this->clazz = raw_type.getClass();
}

Type::Type(const RawType& raw_type, Nature nature, const vector<Type>& element_type) :
	raw_type(raw_type), nature(nature)
{
	this->clazz = raw_type.getClass();
	this->element_types = element_type;
}

bool Type::must_manage_memory() const {
	return nature == Nature::LSVALUE;
}

Type Type::getReturnType() const {
	if (return_types.size() == 0) {
		return Type::UNKNOWN;
	}
	return return_types[0];
}

void Type::setReturnType(const Type& type) {
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::addArgumentType(const Type& type) {
	arguments_types.push_back(type);
}

void Type::setArgumentType(size_t index, const Type& type) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
}

const Type& Type::getArgumentType(size_t index) const {
	if (index >= arguments_types.size()) {
		return Type::UNKNOWN;
	}
	return arguments_types[index];
}

const std::vector<Type>& Type::getArgumentTypes() const {
	return arguments_types;
}

const Type& Type::getElementType(size_t i) const {
	if (i < element_types.size()) {
		return element_types[i];
	}
	return Type::UNKNOWN;
}

void Type::setElementType(size_t index, const Type& type) {
	while (element_types.size() <= index) {
		element_types.push_back(Type::UNKNOWN);
	}
	element_types[index] = type;
}

Type Type::mix(const Type& x) const {
/*
	if (*this == x) return *this;
	if (nature == Nature::LSVALUE || x.nature == Nature::LSVALUE) return Type::POINTER;
	if (raw_type == RawType::FLOAT || x.raw_type == RawType::FLOAT) return Type::FLOAT;
	if (raw_type == RawType::INTEGER || x.raw_type == RawType::INTEGER) return Type::INTEGER;
	*/
	return x;
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type.getJsonName() << "\"";

	if (raw_type == RawType::FUNCTION) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments_types.size(); ++t) {
			if (t > 0) os << ",";
			arguments_types[t].toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		getReturnType().toJson(os);
	}
	os << "}";
}

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			nature == type.nature &&
			clazz == type.clazz &&
			element_types == type.element_types &&
			return_types == type.return_types &&
			arguments_types == type.arguments_types;
}

bool Type::can_be_convert_in(const Type& type) const {
	return get_compatible_type(*this, type) == type;
}

bool Type::is_primitive_number() const
{
	return *this == Type::BOOLEAN || *this == Type::I32 || *this == Type::I64 || *this == Type::F32 || *this == Type::F64;
}

bool Type::list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {

	if (expected.size() != actual.size()) return false;

	for (size_t i = 0; i < expected.size(); ++i) {
		if (!actual[i].can_be_convert_in(expected[i])) return false;
	}
	return true;
}

bool Type::list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww) {

	if (old.size() != neww.size()) return false;

	for (size_t i = 0; i < old.size(); ++i) {
		if (Type::more_specific(old[i], neww[i])) {
			return true;
		}
	}
	return false;
}

bool Type::more_specific(const Type& old, const Type& neww) {
	if (old == Type::UNKNOWN) return true;

	return old != neww && neww.can_be_convert_in(old);
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {
	/* Returns a type into both t1 and t2 can be converted
	 */

	if (t1 == t2) {
		return t1;
	}

	if (t1 == Type::UNKNOWN) return t2;
	if (t2 == Type::UNKNOWN) return t1;

	// FUNCTION
	if (t1.raw_type == RawType::FUNCTION && t2.raw_type == RawType::FUNCTION) {
		if (t1.arguments_types.size() != t2.arguments_types.size()) return Type::VOID;
		vector<Type> compatible_args;
		for (size_t i = 0; i < t1.arguments_types.size(); ++i) {
			compatible_args.push_back(get_compatible_type(t1.arguments_types[i], t2.arguments_types[i]));
			if (compatible_args[i] == Type::VOID) return Type::VOID;
		}
		if (t1.getReturnType() == Type::VOID && t2.getReturnType() == Type::VOID) {
			Type fun = Type::FUNCTION;
			fun.arguments_types = compatible_args;
			fun.setReturnType(Type::VOID);
			return fun;
		}
		if (t1.getReturnType() == Type::VOID || t2.getReturnType() == Type::VOID) return Type::VOID;
		Type compatible_return = get_compatible_type(t1.getReturnType(), t2.getReturnType());
		if (compatible_return == Type::VOID) return Type::VOID;
		Type fun = Type::FUNCTION;
		fun.arguments_types = compatible_args;
		fun.setReturnType(compatible_return);
		return fun;
	}
	if (t1.raw_type == RawType::FUNCTION || t2.raw_type == RawType::FUNCTION) return Type::VOID;

	// VAR
	if (t1 == Type::VAR && (t2 == Type::BOOLEAN || t2 == Type::I32 || t2 == Type::I64 || t2 == Type::F32 || t2 == Type::F64)) return Type::VAR;
	if (t2 == Type::VAR && (t1 == Type::BOOLEAN || t1 == Type::I32 || t1 == Type::I64 || t1 == Type::F32 || t1 == Type::F64)) return Type::VAR;
	if (t1 == Type::VAR || t2 == Type::VAR) return Type::VOID;

	// VEC
	if (t1.raw_type == RawType::VEC && t2.raw_type == RawType::VEC) {
		Type compatible_element = get_compatible_type(t1.getElementType(0), t2.getElementType(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(RawType::VEC, Nature::LSVALUE, { compatible_element });
	}
	if (t1.raw_type == RawType::VEC || t2.raw_type == RawType::VEC) return Type::VOID;

	// SET
	if (t1.raw_type == RawType::SET && t2.raw_type == RawType::SET) {
		Type compatible_element = get_compatible_type(t1.getElementType(0), t2.getElementType(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(RawType::SET, Nature::LSVALUE, { compatible_element });
	}
	if (t1.raw_type == RawType::SET || t2.raw_type == RawType::SET) return Type::VOID;

	// MAP
	if (t1.raw_type == RawType::MAP && t2.raw_type == RawType::MAP) {
		Type compatible_key = get_compatible_type(t1.getElementType(0), t2.getElementType(0));
		if (compatible_key == Type::VOID) return Type::VOID;
		Type compatible_value = get_compatible_type(t1.getElementType(0), t2.getElementType(0));
		if (compatible_value == Type::VOID) return Type::VOID;
		return Type(RawType::MAP, Nature::LSVALUE, { compatible_key, compatible_value });
	}
	if (t1.raw_type == RawType::MAP || t2.raw_type == RawType::MAP) return Type::VOID;

	// TUPLE
	if (t1.raw_type == RawType::TUPLE && t2.raw_type == RawType::TUPLE) {
		if (t1.element_types.size() != t2.element_types.size()) return Type::VOID;
		vector<Type> compatible_elements;
		for (size_t i = 0; i < t1.element_types.size(); ++i) {
			compatible_elements.push_back(get_compatible_type(t1.element_types[i], t2.element_types[i]));
			if (compatible_elements[i] == Type::VOID) return Type::VOID;
		}
		return Type(RawType::TUPLE, Nature::VALUE, compatible_elements);
	}
	if (t1.raw_type == RawType::SET || t2.raw_type == RawType::SET) return Type::VOID;

	// BOOL, I32, I64, F32, F64
	if (t1 == Type::I64 && t2 == Type::I32) return Type::I64;
	if (t2 == Type::I64 && t1 == Type::I32) return Type::I64;
	if (t1 == Type::I64 && t2 == Type::BOOLEAN) return Type::I64;
	if (t2 == Type::I64 && t1 == Type::BOOLEAN) return Type::I64;
	if (t1 == Type::I64 || t2 == Type::I64) return Type::VOID;
	if (t1 == Type::F64 || t2 == Type::F64) return Type::F64;
	if (t1 == Type::I32 && t2 == Type::BOOLEAN) return Type::I32;
	if (t2 == Type::I32 && t1 == Type::BOOLEAN) return Type::I32;
	if (t1 == Type::I32 || t2 == Type::I32) return Type::VOID;

	return Type::VOID;
}

string Type::get_nature_name(const Nature& nature) {
	switch (nature) {
	case Nature::LSVALUE:
		return "POINTER";
	case Nature::UNKNOWN:
		return "UNKNOWN";
	case Nature::VALUE:
		return "VALUE";
	case Nature::VOID:
		return "VOID";
	default:
		return "??";
	}
}

string Type::get_nature_symbol(const Nature& nature) {
	switch (nature) {
	case Nature::LSVALUE:
		return "*";
	case Nature::UNKNOWN:
		return "?";
	case Nature::VALUE:
		return "";
	case Nature::VOID:
		return "void";
	default:
		return "???";
	}
}

ostream& operator << (ostream& os, const Type& type) {

	if (type == Type::VOID) {
		os << "{void}";
		return os;
	}

	os << "{" << type.raw_type.getName() << Type::get_nature_symbol(type.nature);

	if (type.raw_type == RawType::FUNCTION) {
		os << " (";
		for (size_t t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
		}
		os << ") → " << type.getReturnType();
	}
	if (type.raw_type == RawType::VEC || type.raw_type == RawType::SET) {
		os << " of " << type.getElementType();
	}
	if (type.raw_type == RawType::MAP) {
		os << " of " << type.getElementType(0) << " → " << type.getElementType(1);
	}
	os << "}";
	return os;
}



}
