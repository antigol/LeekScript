#include "Type.hpp"
#include "../constants.h"
#include <sstream>
#include "../../lib/utf8.h"

using namespace std;

namespace ls {

BaseRawType::~BaseRawType() {}

const UnknownRawType RawType::_UNKNOWN;
const VoidRawType RawType::_VOID;
const NullRawType RawType::_NULLL;
const BooleanRawType RawType::_BOOLEAN;
const NumberRawType RawType::_NUMBER;
const MpzRawType RawType::_MPZ;
const IntegerRawType RawType::_INTEGER;
const LongRawType RawType::_LONG;
const FloatRawType RawType::_REAL;
const StringRawType RawType::_STRING;
const ArrayRawType RawType::_ARRAY;
const MapRawType RawType::_MAP;
const SetRawType RawType::_SET;
const IntervalRawType RawType::_INTERVAL;
const ObjectRawType RawType::_OBJECT;
const FunctionRawType RawType::_FUNCTION;
const ClassRawType RawType::_CLASS;

const UnknownRawType* const RawType::UNKNOWN = &_UNKNOWN;
const VoidRawType* const RawType::VOID = &_VOID;
const NullRawType* const RawType::NULLL = &_NULLL;
const BooleanRawType* const RawType::BOOLEAN = &_BOOLEAN;
const NumberRawType* const RawType::NUMBER = &_NUMBER;
const IntegerRawType* const RawType::INTEGER = &_INTEGER;
const MpzRawType* const RawType::MPZ = &_MPZ;
const LongRawType* const RawType::LONG = &_LONG;
const FloatRawType* const RawType::REAL = &_REAL;
const StringRawType* const RawType::STRING = &_STRING;
const ArrayRawType* const RawType::ARRAY = &_ARRAY;
const MapRawType* const RawType::MAP = &_MAP;
const SetRawType* const RawType::SET = &_SET;
const IntervalRawType* const RawType::INTERVAL = &_INTERVAL;
const ObjectRawType* const RawType::OBJECT = &_OBJECT;
const FunctionRawType* const RawType::FUNCTION = &_FUNCTION;
const ClassRawType* const RawType::CLASS = &_CLASS;

std::vector<const BaseRawType*> RawType::placeholder_types;

void RawType::clear_placeholder_types() {
	for (const auto& t : placeholder_types)
		delete t;
	placeholder_types.clear();
}

unsigned int Type::placeholder_counter = 0;

const Type Type::UNKNOWN(RawType::UNKNOWN, Nature::UNKNOWN);

const Type Type::VOID(RawType::VOID, Nature::VOID);
const Type Type::VALUE(RawType::UNKNOWN, Nature::VALUE);
const Type Type::POINTER(RawType::UNKNOWN, Nature::POINTER);

const Type Type::NULLL(RawType::NULLL, Nature::POINTER, true);
const Type Type::BOOLEAN(RawType::BOOLEAN, Nature::VALUE);
const Type Type::BOOLEAN_P(RawType::BOOLEAN, Nature::POINTER);
const Type Type::NUMBER_VALUE(RawType::NUMBER, Nature::VALUE);
const Type Type::NUMBER(RawType::NUMBER, Nature::POINTER);
const Type Type::INTEGER(RawType::INTEGER, Nature::VALUE);
const Type Type::MPZ(RawType::MPZ, Nature::VALUE);
const Type Type::MPZ_TMP(RawType::MPZ, Nature::VALUE, false, true);
const Type Type::LONG(RawType::LONG, Nature::VALUE);
const Type Type::REAL(RawType::REAL, Nature::VALUE);
const Type Type::STRING(RawType::STRING, Nature::POINTER);
const Type Type::STRING_TMP(RawType::STRING, Nature::POINTER, false, true);
const Type Type::OBJECT(RawType::OBJECT, Nature::POINTER);
const Type Type::ARRAY(RawType::ARRAY, Nature::POINTER);
const Type Type::PTR_ARRAY(RawType::ARRAY, Nature::POINTER, Type::POINTER);
const Type Type::INT_ARRAY(RawType::ARRAY, Nature::POINTER, Type::INTEGER);
const Type Type::REAL_ARRAY(RawType::ARRAY, Nature::POINTER, Type::REAL);
const Type Type::STRING_ARRAY(RawType::ARRAY, Nature::POINTER, Type::STRING);
const Type Type::MAP(RawType::MAP, Nature::POINTER);
const Type Type::PTR_PTR_MAP(RawType::MAP, Nature::POINTER, Type::POINTER, Type::POINTER);
const Type Type::PTR_INT_MAP(RawType::MAP, Nature::POINTER, Type::POINTER, Type::INTEGER);
const Type Type::PTR_REAL_MAP(RawType::MAP, Nature::POINTER, Type::POINTER, Type::REAL);
const Type Type::REAL_PTR_MAP(RawType::MAP, Nature::POINTER, Type::REAL, Type::POINTER);
const Type Type::REAL_INT_MAP(RawType::MAP, Nature::POINTER, Type::REAL, Type::INTEGER);
const Type Type::REAL_REAL_MAP(RawType::MAP, Nature::POINTER, Type::REAL, Type::REAL);
const Type Type::INT_PTR_MAP(RawType::MAP, Nature::POINTER, Type::INTEGER, Type::POINTER);
const Type Type::INT_INT_MAP(RawType::MAP, Nature::POINTER, Type::INTEGER, Type::INTEGER);
const Type Type::INT_REAL_MAP(RawType::MAP, Nature::POINTER, Type::INTEGER, Type::REAL);
const Type Type::SET(RawType::SET, Nature::POINTER);
const Type Type::PTR_SET(RawType::SET, Nature::POINTER, Type::POINTER);
const Type Type::INT_SET(RawType::SET, Nature::POINTER, Type::INTEGER);
const Type Type::REAL_SET(RawType::SET, Nature::POINTER, Type::REAL);
const Type Type::INTERVAL(RawType::INTERVAL, Nature::POINTER, Type::INTEGER);
const Type Type::PTR_ARRAY_ARRAY(RawType::ARRAY, Nature::POINTER, Type::PTR_ARRAY);
const Type Type::REAL_ARRAY_ARRAY(RawType::ARRAY, Nature::POINTER, Type::REAL_ARRAY);
const Type Type::INT_ARRAY_ARRAY(RawType::ARRAY, Nature::POINTER, Type::INT_ARRAY);

const Type Type::FUNCTION(RawType::FUNCTION, Nature::VALUE);
const Type Type::FUNCTION_P(RawType::FUNCTION, Nature::POINTER);
const Type Type::CLASS(RawType::CLASS, Nature::POINTER, true);

const Type Type::STRING_ITERATOR(RawType::STRING, Nature::VALUE, Type::STRING);
const Type Type::INTERVAL_ITERATOR(RawType::INTERVAL, Nature::VALUE, Type::INTEGER);
const Type Type::SET_ITERATOR(RawType::SET, Nature::VALUE, Type::UNKNOWN);
const Type Type::INTEGER_ITERATOR(RawType::INTEGER, Nature::VALUE, Type::INTEGER);
const Type Type::LONG_ITERATOR(RawType::LONG, Nature::VALUE, Type::INTEGER);
const Type Type::MPZ_ITERATOR(RawType::MPZ, Nature::VALUE, Type::INTEGER);

Type::Type() {
	raw_type = RawType::UNKNOWN;
	nature = Nature::UNKNOWN;
	native = false;
	clazz = "?";
}

Type::Type(const BaseRawType* raw_type, Nature nature, bool native, bool temporary) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
	this->temporary = temporary;
}

Type::Type(const BaseRawType* raw_type, Nature nature, const Type& element_type, bool native) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
	this->setElementType(element_type);
}

Type::Type(const BaseRawType* raw_type, Nature nature, const Type& key_type, const Type& element_type, bool native) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
	this->setKeyType(key_type);
	this->setElementType(element_type);
}

int Type::id() const {
	return raw_type->id();
}

bool Type::must_manage_memory() const {
	return nature == Nature::POINTER and not native;
}

Type Type::getReturnType() const {
	if (return_types.size() == 0) {
		return Type::UNKNOWN;
	}
	return return_types[0];
}

void Type::setReturnType(Type type) {
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::addArgumentType(Type type) {
	arguments_types.push_back(type);
}

void Type::setArgumentType(size_t index, Type type, bool has_default) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;

	while (arguments_has_default.size() <= index) {
		arguments_has_default.push_back(false);
	}
	arguments_has_default[index] = has_default;
}

/*
 * By default, all arguments are type INTEGER, but if we see it's not always
 * a integer, it will switch to UNKNOWN
 */
const Type& Type::getArgumentType(size_t index) const {
	if (index >= arguments_types.size()) {
		return Type::UNKNOWN;
	}
	return arguments_types[index];
}

const std::vector<Type>& Type::getArgumentTypes() const {
	return arguments_types;
}

bool Type::argumentHasDefault(size_t index) const {
	if (index >= arguments_has_default.size()) {
		return false;
	}
	return arguments_has_default[index];
}

const Type& Type::getElementType() const {
	if (element_type.size()) {
		return element_type[0];
	}
	return Type::UNKNOWN;
}

const Type& Type::element() const {
	return getElementType();
}

void Type::setElementType(const Type& type) {
	if (element_type.size() == 0) {
		element_type.push_back(type);
	} else {
		element_type[0] = type;
	}
}

const Type& Type::getKeyType() const {
	if (key_type.size()) {
		return key_type[0];
	}
	assert(false); // LCOV_EXCL_LINE
}
void Type::setKeyType(const Type& type) {
	if (key_type.size() == 0) {
		key_type.push_back(type);
	} else {
		key_type[0] = type;
	}
}

bool Type::will_take(const std::vector<Type>& args_type) {

	bool changed = false;

	for (size_t i = 0; i < args_type.size(); ++i) {

		Type current_type = getArgumentType(i);

		if (current_type.nature == Nature::UNKNOWN) {
			setArgumentType(i, args_type[i]);
			changed = true;
		} else {
			if (current_type.nature == Nature::VALUE and args_type[i].nature == Nature::POINTER) {
				setArgumentType(i, Type(RawType::UNKNOWN, Nature::POINTER));
				changed = true;
			}
		}
	}
	return changed;
}

Type Type::mix(const Type& x) const {
	if (*this == x) return *this;
	if (*this == Type::UNKNOWN or x == Type::UNKNOWN) return Type::UNKNOWN;
	if (nature == Nature::POINTER || x.nature == Nature::POINTER) return Type::POINTER;
	if (nature == Nature::VOID || x.nature == Nature::VOID) return Type::POINTER;
	if (raw_type == RawType::REAL || x.raw_type == RawType::REAL) return Type::REAL;
	if (raw_type == RawType::LONG || x.raw_type == RawType::LONG) return Type::LONG;
	if (raw_type == RawType::INTEGER || x.raw_type == RawType::INTEGER) return Type::INTEGER;
	assert(false); // LCOV_EXCL_LINE
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type->getJsonName() << "\"";

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

std::string Type::to_string() const {
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

bool Type::isNumber() const {
	return dynamic_cast<const NumberRawType*>(raw_type) != nullptr;
}

bool Type::iterable() const {
	return raw_type->iterable();
}

bool Type::is_container() const {
	return raw_type->is_container();
}

int Type::size() const {
	if (nature == Nature::POINTER) {
		return 64;
	}
	return raw_type->size();
}

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			nature == type.nature &&
			native == type.native &&
			clazz == type.clazz &&
			element_type == type.element_type &&
			key_type == type.key_type &&
			temporary == type.temporary &&
			(raw_type != RawType::FUNCTION ||
				(return_types == type.return_types &&
				arguments_types == type.arguments_types));
}

bool Type::operator < (const Type& type) const {
	if ((void*) raw_type != (void*) type.raw_type) {
		return (void*) raw_type < (void*) type.raw_type;
	}
	if (nature != type.nature) {
		return nature < type.nature;
	}
	if (temporary != type.temporary) {
		return temporary < type.temporary;
	}
	if (native != type.native) {
		return native < type.native;
	}
	for (size_t i = 0; i < element_type.size(); ++i) {
		if (i >= type.element_type.size()) return false;
		if (element_type.at(i) != type.element_type.at(i)) {
			return element_type.at(i) < type.element_type.at(i);
		}
	}
	for (size_t i = 0; i < key_type.size(); ++i) {
		if (i >= type.key_type.size()) return false;
		if (key_type.at(i) != type.key_type.at(i)) {
			return key_type.at(i) < type.key_type.at(i);
		}
	}
	return false;
}

Type Type::not_temporary() const {
	Type new_type = *this;
	new_type.temporary = false;
	return new_type;
}

/*
 * Can we convert type into this ?
 * {float}.compatible({int}) == true
 * {int*}.compatible({int}) == true
 * {int}.compatible({float}) == false
 * {int}.compatible({int*}) == false
 */
bool Type::compatible(const Type& type) const {

	if (this->nature == Nature::VALUE && type.nature == Nature::POINTER) {
		return false;
	}

	if (this->temporary and not type.temporary) {
		return false; // type not compatible with type&&
	}

	if (this->raw_type != type.raw_type) {

		// Every type is compatible with 'Unknown' type
		if (this->raw_type == RawType::UNKNOWN) {
			return true;
		}

		// 'Integer' is compatible with 'Float'
		if (this->raw_type == RawType::REAL and type.raw_type == RawType::INTEGER) {
			return true;
		}

		// 'Integer' is compatible with 'Long'
		if (this->raw_type == RawType::LONG and type.raw_type == RawType::INTEGER) {
			return true;
		}

		// All numbers types are compatible with the base 'Number' type
		if (this->raw_type == RawType::NUMBER and (
			type.raw_type == RawType::INTEGER or
			type.raw_type == RawType::LONG or
			type.raw_type == RawType::REAL
		)) return true;

		return false;
	}

	if (this->raw_type == RawType::ARRAY || this->raw_type == RawType::SET) {
		const Type& e1 = this->getElementType();
		const Type& e2 = type.getElementType();
		if (e1 == Type::UNKNOWN) {
			return true;
		}
		if (e1.nature == Nature::POINTER && e2.nature == Nature::POINTER) return true;
		return e1 == e2;
	}

	if (this->raw_type == RawType::MAP) {
		const Type& k1 = this->getKeyType();
		const Type& k2 = type.getKeyType();
		const Type& v1 = this->getElementType();
		const Type& v2 = type.getElementType();
		if (k1.nature == Nature::POINTER && k2.nature == Nature::POINTER) {
			if (v1.nature == Nature::POINTER && v2.nature == Nature::POINTER) {
				return true;
			}
			return v1 == v2;
		} else {
			if (v1.nature == Nature::POINTER && v2.nature == Nature::POINTER) {
				return k1 == k2;
			}
			return k1 == k2 && v1 == v2;
		}
	}

	return true;
}

bool Type::list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {

	if (expected.size() != actual.size()) return false;

	for (size_t i = 0; i < expected.size(); ++i) {
		// Can we convert type actual[i] into type expected[i] ?
		if (not expected[i].compatible(actual[i])) return false;
	}
	return true;
}

bool Type::may_be_compatible(const Type& type) const {
	if (compatible(type)) {
		return true;
	}
	// Example: Number.abs(number*) => we allow to call with a unknown pointer
	if (this->nature == Nature::POINTER and type == Type::POINTER) {
		return true;
	}
	return false;
}

bool Type::list_may_be_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {
	if (expected.size() != actual.size()) return false;
	for (size_t i = 0; i < expected.size(); ++i) {
		if (not expected[i].may_be_compatible(actual[i])) return false;
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

	if (neww.raw_type != old.raw_type) {
		if (old.raw_type == RawType::UNKNOWN) {
			return true;
		}
		if (old.raw_type == RawType::NUMBER
				&& (neww.raw_type == RawType::INTEGER || neww.raw_type == RawType::LONG || neww.raw_type == RawType::REAL)) {
			return true;
		}
		if (old.raw_type == RawType::REAL
				&& (neww.raw_type == RawType::INTEGER || neww.raw_type == RawType::LONG)) {
			return true;
		}
		if (old.raw_type == RawType::LONG
				&& neww.raw_type == RawType::INTEGER) {
			return true;
		}
	}

	if ((neww.raw_type == RawType::ARRAY and old.raw_type == RawType::ARRAY)
			|| (neww.raw_type == RawType::SET and old.raw_type == RawType::SET)) {
		if (Type::more_specific(old.getElementType(), neww.getElementType())) {
			return true;
		}
	}

	if (neww.raw_type == RawType::MAP and old.raw_type == RawType::MAP) {
		if (Type::more_specific(old.getKeyType(), neww.getKeyType())
				|| Type::more_specific(old.getElementType(), neww.getElementType())) {
			return true;
		}
	}

	if (neww.raw_type == RawType::FUNCTION and old.raw_type == RawType::FUNCTION) {
		if (Type::more_specific(old.getArgumentType(0), neww.getArgumentType(0))) { //! TODO only the first arg
			return true;
		}
	}

	if (not old.temporary and neww.temporary) {
		return true;
	}

	return false;
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {
	if (t1 == t2) {
		return t1;
	}
	if (t1.nature == Nature::POINTER and t2.nature == Nature::VALUE) {
		return Type::POINTER;
	}
	if (t2.nature == Nature::POINTER and t1.nature == Nature::VALUE) {
		return Type::POINTER;
	}
	if (t1.raw_type == RawType::UNKNOWN) {
		return t2;
	}
	if (t2.raw_type == RawType::UNKNOWN) {
		return t1;
	}
	if (t2.compatible(t1)) {
		return t2;
	}
	if (t1.compatible(t2)) {
		return t1;
	}
	return Type::POINTER;
}

string Type::get_nature_symbol(const Nature& nature) {
	switch (nature) {
	case Nature::POINTER:
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

Type Type::generate_new_placeholder_type() {
	Type type;
	type.nature = Nature::VALUE;
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	type.raw_type = new PlaceholderRawType(std::string { buff });
	placeholder_counter++;
	RawType::placeholder_types.push_back(type.raw_type);
	return type;
}

ostream& operator << (ostream& os, const Type& type) {

	if (type == Type::VOID) {
		os << GREY << "{void}" << END_COLOR;
		return os;
	}

	auto color = (type.nature == Nature::VALUE) ? GREEN : RED;
	os << color;

	if (type.raw_type == RawType::UNKNOWN) {
		if (type.nature == Nature::POINTER) {
			os << YELLOW << "*";
		} else {
			os << YELLOW << "?";
		}
	} else if (type.raw_type == RawType::FUNCTION) {
		if (type.nature == Nature::POINTER) {
			os << BLUE_BOLD;
		}
		os << "fun(";
		for (unsigned t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
			if (type.nature == Nature::POINTER) {
				os << BLUE_BOLD;
			}
		}
		if (type.nature == Nature::POINTER) {
			os << BLUE_BOLD;
		}
		os << ") → " << type.getReturnType();
	} else if (type.raw_type == RawType::STRING || type.raw_type == RawType::CLASS
		|| type.raw_type == RawType::OBJECT || type.raw_type == RawType::NULLL
		|| type.raw_type == RawType::INTERVAL) {
		os << BLUE_BOLD << type.raw_type->getName(); // << Type::get_nature_symbol(type.nature);
	} else if (type.raw_type == RawType::ARRAY || type.raw_type == RawType::SET) {
		os << BLUE_BOLD << type.raw_type->getName(); // << Type::get_nature_symbol(type.nature);
		os << "<" << type.getElementType() << BLUE_BOLD << ">";
	} else if (type.raw_type == RawType::MAP) {
		os << BLUE_BOLD << type.raw_type->getName();
		os << "<" << type.getKeyType() << BLUE_BOLD
			<< ", " << type.getElementType() << BLUE_BOLD << ">";
	} else {
		os << type.raw_type->getName() << Type::get_nature_symbol(type.nature);
	}
	if (type.temporary && type != Type::REAL) {
		os << "&&";
	} else if (type.reference) {
		os << "&";
	}
	os << END_COLOR;

	return os;
}

ostream& operator << (ostream& os, const std::vector<Type>& types) {
	os << "[";
	for (unsigned i = 0; i < types.size(); ++i) {
		if (i > 0) os << ", ";
		os << types[i];
	}
	os << "]";
	return os;
}

}
