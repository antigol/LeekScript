#include "Type.hpp"
#include "../constants.h"
#include <sstream>

using namespace std;

namespace ls {

const BaseRawType* const RawType::UNKNOWN = new BaseRawType();
const VoidRawType* const RawType::VOID = new VoidRawType();
const NullRawType* const RawType::NULLL = new NullRawType();
const BooleanRawType* const RawType::BOOLEAN = new BooleanRawType();
const NumberRawType* const RawType::NUMBER = new NumberRawType();
const IntegerRawType* const RawType::INTEGER = new IntegerRawType();
const IntegerRawType* const RawType::UNSIGNED_INTEGER = new IntegerRawType();
const GmpIntRawType* const RawType::GMP_INT = new GmpIntRawType();
const LongRawType* const RawType::LONG = new LongRawType();
const LongRawType* const RawType::UNSIGNED_LONG = new LongRawType();
const FloatRawType* const RawType::REAL = new FloatRawType();
const StringRawType* const RawType::STRING = new StringRawType();
const ArrayRawType* const RawType::ARRAY = new ArrayRawType();
const MapRawType* const RawType::MAP = new MapRawType();
const SetRawType* const RawType::SET = new SetRawType();
const IntervalRawType* const RawType::INTERVAL = new IntervalRawType();
const ObjectRawType* const RawType::OBJECT = new ObjectRawType();
const FunctionRawType* const RawType::FUNCTION = new FunctionRawType();
const ClassRawType* const RawType::CLASS = new ClassRawType();

const Type Type::UNKNOWN(RawType::UNKNOWN, Nature::UNKNOWN);

const Type Type::VOID(RawType::VOID, Nature::VOID);
const Type Type::VALUE(RawType::UNKNOWN, Nature::VALUE);
const Type Type::POINTER(RawType::UNKNOWN, Nature::POINTER);

const Type Type::NULLL(RawType::NULLL, Nature::POINTER, true);
const Type Type::BOOLEAN(RawType::BOOLEAN, Nature::VALUE);
const Type Type::BOOLEAN_P(RawType::BOOLEAN, Nature::POINTER);
const Type Type::NUMBER_VALUE(RawType::NUMBER, Nature::VALUE);
const Type Type::NUMBER(RawType::NUMBER, Nature::POINTER);
const Type Type::UNSIGNED_INTEGER(RawType::UNSIGNED_INTEGER, Nature::VALUE);
const Type Type::INTEGER(RawType::INTEGER, Nature::VALUE);
const Type Type::GMP_INT(RawType::GMP_INT, Nature::VALUE);
const Type Type::GMP_INT_TMP(RawType::GMP_INT, Nature::VALUE, false, true);
const Type Type::LONG(RawType::LONG, Nature::VALUE);
const Type Type::UNSIGNED_LONG(RawType::UNSIGNED_LONG, Nature::VALUE);
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

void Type::setArgumentType(size_t index, Type type) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
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

const Type& Type::getElementType() const {
	if (element_type.size()) {
		return element_type[0];
	}
	return Type::UNKNOWN;
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
	return Type::UNKNOWN;
}
void Type::setKeyType(const Type& type) {
	if (key_type.size() == 0) {
		key_type.push_back(type);
	} else {
		key_type[0] = type;
	}
}

/*
 *
 */
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

bool Type::will_take_element(const Type& element_type) {

	if (raw_type != RawType::ARRAY) {
		return false;
	}

	Type current = getElementType();

	if (current == element_type) {
		return false;
	}

	setElementType(element_type);
	return true;
}

Type Type::mix(const Type& x) const {

	if (*this == x) return *this;
	if (*this == Type::UNKNOWN or x == Type::UNKNOWN) return Type::UNKNOWN;
	if (nature == Nature::POINTER || x.nature == Nature::POINTER) return Type::POINTER;
	if (raw_type == RawType::REAL || x.raw_type == RawType::REAL) return Type::REAL;
	if (raw_type == RawType::INTEGER || x.raw_type == RawType::INTEGER) return Type::INTEGER;
	return x;
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

std::string Type::toString() const {
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

bool Type::isNumber() const {
	return dynamic_cast<const NumberRawType*>(raw_type) != nullptr;
}

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			nature == type.nature &&
			native == type.native &&
			clazz == type.clazz &&
			element_type == type.element_type &&
			key_type == type.key_type &&
			return_types == type.return_types &&
			temporary == type.temporary &&
			arguments_types == type.arguments_types;
}

/*
 * Can we convert type into this ?
 * {float}.compatible({int}) == true
 * {int*}.compatible({int}) == true
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
		if (t1.raw_type == t2.raw_type) {
			if (t1.element_type == t2.element_type
					&& t1.key_type == t2.key_type
					&& t1.return_types == t2.return_types
					&& t1.arguments_types == t2.arguments_types) {
				return t1; // They are identical except the Nature
			}
			return Type(t1.raw_type, Nature::POINTER); // They have the same raw_type : for example {function* ({int})->{int}} and {function ({int})->{void}}
		}
		return Type::POINTER;
	}

	// symmetric of last it statement
	if (t2.nature == Nature::POINTER and t1.nature == Nature::VALUE) {
		if (t2.raw_type == t1.raw_type) {
			if (t2.element_type == t1.element_type
					&& t1.key_type == t2.key_type
					&& t2.return_types == t1.return_types
					&& t2.arguments_types == t1.arguments_types) {
				return t2;
			}
			return Type(t2.raw_type, Nature::POINTER);
		}
		return Type::POINTER;
	}

	if (t1.raw_type == RawType::UNKNOWN) {
		return t2;
	}
	if (t2.raw_type == RawType::UNKNOWN) {
		return t1;
	}

	if (t1.compatible(t2)) {
		return t1;
	}
	if (t2.compatible(t1)) {
		return t2;
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

#if PRINT_TYPES_COLORS
	#define GREY "\033[0;90m"
	#define GREEN "\033[0;32m"
	#define RED "\033[1;31m"
	#define BLUE "\033[1;34m"
	#define YELLOW "\033[1;33m"
	#define END_COLOR "\033[0m"
#else
	#define GREY ""
	#define GREEN ""
	#define RED ""
	#define BLUE ""
	#define YELLOW ""
	#define END_COLOR ""
#endif

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
		os << BLUE << "fun(";
		for (unsigned t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
			os << BLUE;
		}
		os << ") → " << type.getReturnType();
	} else if (type.raw_type == RawType::STRING || type.raw_type == RawType::CLASS
		|| type.raw_type == RawType::OBJECT || type.raw_type == RawType::NULLL
		|| type.raw_type == RawType::INTERVAL) {
		os << BLUE << type.raw_type->getName(); // << Type::get_nature_symbol(type.nature);
	} else if (type.raw_type == RawType::ARRAY || type.raw_type == RawType::SET) {
		os << BLUE << type.raw_type->getName(); // << Type::get_nature_symbol(type.nature);
		os << "<" << type.getElementType() << BLUE << ">";
	} else if (type.raw_type == RawType::MAP) {
		os << BLUE << type.raw_type->getName();
		os << "<" << type.getKeyType() << BLUE
			<< ", " << type.getElementType() << BLUE << ">";
	} else {
		os << type.raw_type->getName() << Type::get_nature_symbol(type.nature);
	}
	os << END_COLOR;

	if (type.temporary && type != Type::REAL) {
		os << "&&";
	}

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
