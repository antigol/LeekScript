#include "Type.hpp"
#include <cassert>
#include <algorithm>
#include <set>

using namespace std;

namespace ls {

RawType::RawType(const string& name, const string& classname, const string& jsonname, size_t bytes, jit_type_t jit_type, Nature nature, int id)
	: _name(name), _clazz(classname), _json_name(jsonname), _bytes(bytes), _jit_type(jit_type), _nature(nature), id(id)
{}

bool RawType::operator ==(const RawType& type) const {
	return id == type.id;
//	return _name == type._name && _clazz == type._clazz && _json_name == type._json_name && _bytes == type._bytes && _jit_type == type._jit_type && _nature == type._nature;
}

bool RawType::operator <(const RawType& type) const
{
	return id < type.id;
}

const RawType RawType::UNKNOWN    ("?",           "?",        "?",        0,                nullptr,           Nature::UNKNOWN,  11);
const RawType RawType::VOID       ("void",        "?",        "void",     0,                jit_type_void,     Nature::VOID,     12);
const RawType RawType::UNREACHABLE("unreachable", "?",        "",         0,                nullptr,           Nature::VOID,     13);
const RawType RawType::LSVALUE    ("lsvalue",     "?",        "?",         sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE, 14);
const RawType RawType::VAR        ("var",         "Variable", "variable", sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  5);
const RawType RawType::BOOLEAN    ("bool",        "Boolean",  "boolean",  sizeof (int32_t), jit_type_int,      Nature::VALUE,    0);
const RawType RawType::I32        ("i32",         "Number",   "number",   sizeof (int32_t), jit_type_int,      Nature::VALUE,    1);
const RawType RawType::I64        ("i64",         "Number",   "number",   sizeof (int64_t), jit_type_long,     Nature::VALUE,    2);
const RawType RawType::F32        ("f32",         "Number",   "number",   sizeof (float),   jit_type_float32,  Nature::VALUE,    3);
const RawType RawType::F64        ("f64",         "Number",   "number",   sizeof (double),  jit_type_float64,  Nature::VALUE,    4);
const RawType RawType::VEC        ("vec",         "Vec",      "vec",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  6);
const RawType RawType::MAP        ("map",         "Map",      "map",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  7);
const RawType RawType::SET        ("set",         "Set",      "set",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  8);
const RawType RawType::FUNCTION   ("fn",          "?",        "fn",       sizeof (void*),   jit_type_void_ptr, Nature::VALUE,    9);
const RawType RawType::TUPLE      ("tuple",       "Tuple",    "tuple",    0,                nullptr,           Nature::VALUE,    10);

const Type Type::UNKNOWN    (RawType::UNKNOWN);
const Type Type::LSVALUE    (RawType::LSVALUE);
const Type Type::VOID       (RawType::VOID);
const Type Type::UNREACHABLE(RawType::UNREACHABLE);
const Type Type::VAR        (RawType::VAR);
const Type Type::BOOLEAN    (RawType::BOOLEAN);
const Type Type::I32        (RawType::I32);
const Type Type::I64        (RawType::I64);
const Type Type::F32        (RawType::F32);
const Type Type::F64        (RawType::F64);
const Type Type::VEC        (RawType::VEC, { Type::UNKNOWN });
const Type Type::VEC_VAR    (RawType::VEC, { Type::VAR });
const Type Type::VEC_I32    (RawType::VEC, { Type::I32 });
const Type Type::VEC_F64    (RawType::VEC, { Type::F64 });
const Type Type::MAP        (RawType::VEC, { Type::UNKNOWN, Type::UNKNOWN });
const Type Type::SET        (RawType::VEC, { Type::UNKNOWN });
const Type Type::FUNCTION   (RawType::FUNCTION);
const Type Type::TUPLE      (RawType::TUPLE);

Type::Type() :
	raw_type(RawType::UNKNOWN), ph(0)
{}

Type::Type(const RawType& raw_type) :
	raw_type(raw_type), ph(0)
{}

Type::Type(const RawType& raw_type, const vector<Type>& elements_types) :
	raw_type(raw_type), elements_types(elements_types), ph(0)
{}

Type Type::place_holder(int id) const
{
	Type type = *this;
	type.ph = id;
	return type;
}

bool Type::must_manage_memory() const {
	return raw_type.nature() == Nature::LSVALUE;
}

Type Type::return_type() const {
	if (return_types.size() == 0) {
		return Type::UNKNOWN;
	}
	return return_types[0];
}

void Type::set_return_type(const Type& type) {
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::add_argument_type(const Type& type) {
	arguments_types.push_back(type);
}

void Type::set_argument_type(size_t index, const Type& type) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
}

const Type& Type::argument_type(size_t index) const {
	if (index >= arguments_types.size()) {
		return Type::UNKNOWN;
	}
	return arguments_types[index];
}

const Type& Type::element_type(size_t i) const {
	if (i < elements_types.size()) {
		return elements_types[i];
	}
	return Type::UNKNOWN;
}

void Type::set_element_type(size_t index, const Type& type) {
	while (elements_types.size() <= index) {
		elements_types.push_back(Type::UNKNOWN);
	}
	elements_types[index] = type;
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type.json_name() << "\"";

	if (raw_type == RawType::FUNCTION) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments_types.size(); ++t) {
			if (t > 0) os << ",";
			arguments_types[t].toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		return_type().toJson(os);
	}
	os << "}";
}

bool Type::can_be_convert_in(const Type& type) const {
	return get_compatible_type(*this, type) == type;
}

bool Type::is_primitive_number() const
{
	if (raw_type == RawType::UNKNOWN) {
		for (const Type& type : elements_types) if (type.is_primitive_number()) return true;
	}
	return *this == Type::BOOLEAN || *this == Type::I32 || *this == Type::I64 || *this == Type::F32 || *this == Type::F64;
}

bool Type::is_arithmetic() const
{
	if (raw_type == RawType::UNKNOWN) {
		for (const Type& type : elements_types) if (type.is_arithmetic()) return true;
	}
	return *this == Type::VAR || *this == Type::BOOLEAN || *this == Type::I32 || *this == Type::I64 || *this == Type::F32 || *this == Type::F64;
}

bool Type::is_complete() const
{
	if (raw_type == RawType::UNKNOWN || raw_type == RawType::LSVALUE) return false;
	for (const Type& x : elements_types)  if (!x.is_complete()) return false;
	for (const Type& x : return_types)    if (!x.is_complete()) return false;
	for (const Type& x : arguments_types) if (!x.is_complete()) return false;
	return true;
}

void Type::make_it_complete()
{
	if (raw_type == RawType::UNKNOWN) {
		if (elements_types.empty()) {
			*this = Type::I32;
		} else {
			*this = elements_types[0];
			make_it_complete();
		}
		return;
	}
	if (*this == RawType::LSVALUE) {
		*this = Type::VAR;
		return;
	}
	for (Type& x : elements_types)  x.make_it_complete();
	for (Type& x : return_types)    x.make_it_complete();
	for (Type& x : arguments_types) x.make_it_complete();

	assert(is_complete());
}

void Type::replace_place_holder(int id, const Type& type)
{
	if (ph == id) {
		*this = type;
	}
	for (Type& x : elements_types)  x.replace_place_holder(id, type);
	for (Type& x : return_types)    x.replace_place_holder(id, type);
	for (Type& x : arguments_types) x.replace_place_holder(id, type);
}

bool Type::match_with_generic(const Type& generic, Type* new_generic) const
{
	Type copy = generic;
	bool r = match_with_generic_private(copy, copy);
	if (new_generic) *new_generic = copy;
	return r;
}

bool Type::match_with_generic_private(Type& generic, Type& complete) const
{
	if (*this == Type::UNKNOWN) return true;
	if (raw_type == RawType::UNKNOWN) {
		Type copy = complete;
		for (const Type& type : elements_types) {
			if (type.match_with_generic_private(generic, complete)) return true;
			complete = copy;
		}
		return false;
	}
	if (*this == Type::LSVALUE && generic.raw_type.nature() == Nature::LSVALUE) return true;
	if (generic == Type::UNKNOWN) {
		if (generic.ph > 0) complete.replace_place_holder(generic.ph, *this);
		else generic = *this;
		return true;
	}
	if (generic.raw_type == RawType::UNKNOWN) {
		Type copy = complete;
		for (Type& type : generic.elements_types) {
			if (match_with_generic_private(type, complete)) {
				generic = type;
				return true;
			}
			complete = copy;
		}
		return false;
	}
	if (generic == Type::LSVALUE) {
		if (generic.ph > 0) complete.replace_place_holder(generic.ph, *this);
		else generic = *this;
		return raw_type.nature() == Nature::LSVALUE;
	}

	if (generic.raw_type == raw_type) {
		if (raw_type == RawType::VEC) {
			return element_type(0).match_with_generic_private(generic.elements_types[0], complete);
		}
		if (raw_type == RawType::MAP) {
			return element_type(0).match_with_generic_private(generic.elements_types[0], complete) && element_type(1).match_with_generic_private(generic.elements_types[1], complete);
		}
		if (raw_type == RawType::SET) {
			return element_type(0).match_with_generic_private(generic.elements_types[0], complete);
		}
		if (raw_type == RawType::FUNCTION) {
			if (arguments_types.size() != generic.arguments_types.size()) return false;
			for (size_t i = 0; i < arguments_types.size(); ++i) {
				if (!argument_type(i).match_with_generic_private(generic.arguments_types[i], complete)) return false;
			}
			return return_type().match_with_generic_private(generic.return_types[0], complete);
		}
		if (raw_type == RawType::TUPLE) {
			if (elements_types.size() != generic.elements_types.size()) return false;
			for (size_t i = 0; i < elements_types.size(); ++i) {
				if (!element_type(i).match_with_generic_private(generic.arguments_types[i], complete)) return false;
			}
			return true;
		}
	}

	return *this == generic;
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {
	/* Returns a type into both t1 and t2 can be converted
	 */

	if (t1 == t2) {
		return t1;
	}

	if (t1 == Type::UNKNOWN) return t2;
	if (t2 == Type::UNKNOWN) return t1;

	if (t1.raw_type == RawType::UNKNOWN) {
		set<Type> compatibles;
		for (const Type& type : t1.elements_types) {
			Type compatible = get_compatible_type(t2, type);
			if (compatible.raw_type == RawType::UNKNOWN) {
				compatibles.insert(compatible.elements_types.begin(), compatible.elements_types.end());
			} else if (compatible != Type::VOID) compatibles.insert(compatible);
		}
		if (compatibles.empty()) return Type::VOID;
		if (compatibles.size() == 1) return *compatibles.begin();
		Type type = Type::UNKNOWN;
		type.elements_types.insert(type.elements_types.begin(), compatibles.begin(), compatibles.end());
		return type;
	}
	if (t2.raw_type == RawType::UNKNOWN) return get_compatible_type(t2, t1);

	// FUNCTION
	if (t1.raw_type == RawType::FUNCTION && t2.raw_type == RawType::FUNCTION) {
		if (t1.arguments_types.size() != t2.arguments_types.size()) return Type::VOID;
		vector<Type> compatible_args;
		for (size_t i = 0; i < t1.arguments_types.size(); ++i) {
			compatible_args.push_back(get_compatible_type(t1.arguments_types[i], t2.arguments_types[i]));
			if (compatible_args[i] == Type::VOID) return Type::VOID;
		}
		if (t1.return_type() == Type::VOID && t2.return_type() == Type::VOID) {
			Type fun = Type::FUNCTION;
			fun.arguments_types = compatible_args;
			fun.set_return_type(Type::VOID);
			return fun;
		}
		if (t1.return_type() == Type::VOID || t2.return_type() == Type::VOID) return Type::VOID;
		Type compatible_return = get_compatible_type(t1.return_type(), t2.return_type());
		if (compatible_return == Type::VOID) return Type::VOID;
		Type fun = Type::FUNCTION;
		fun.arguments_types = compatible_args;
		fun.set_return_type(compatible_return);
		return fun;
	}
	if (t1.raw_type == RawType::FUNCTION || t2.raw_type == RawType::FUNCTION) return Type::VOID;

	// VAR
	if (t1 == Type::VAR && (t2 == Type::BOOLEAN || t2 == Type::I32 || t2 == Type::I64 || t2 == Type::F32 || t2 == Type::F64)) return Type::VAR;
	if (t2 == Type::VAR && (t1 == Type::BOOLEAN || t1 == Type::I32 || t1 == Type::I64 || t1 == Type::F32 || t1 == Type::F64)) return Type::VAR;
	if (t1 == Type::VAR || t2 == Type::VAR) return Type::VOID;

	// VEC
	if (t1.raw_type == RawType::VEC && t2.raw_type == RawType::VEC) {
		Type compatible_element = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(RawType::VEC, { compatible_element });
	}
	if (t1.raw_type == RawType::VEC || t2.raw_type == RawType::VEC) return Type::VOID;

	// SET
	if (t1.raw_type == RawType::SET && t2.raw_type == RawType::SET) {
		Type compatible_element = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(RawType::SET, { compatible_element });
	}
	if (t1.raw_type == RawType::SET || t2.raw_type == RawType::SET) return Type::VOID;

	// MAP
	if (t1.raw_type == RawType::MAP && t2.raw_type == RawType::MAP) {
		Type compatible_key = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_key == Type::VOID) return Type::VOID;
		Type compatible_value = get_compatible_type(t1.element_type(1), t2.element_type(1));
		if (compatible_value == Type::VOID) return Type::VOID;
		return Type(RawType::MAP, { compatible_key, compatible_value });
	}
	if (t1.raw_type == RawType::MAP || t2.raw_type == RawType::MAP) return Type::VOID;

	// TUPLE
	if (t1.raw_type == RawType::TUPLE && t2.raw_type == RawType::TUPLE) {
		if (t1.elements_types.size() != t2.elements_types.size()) return Type::VOID;
		vector<Type> compatible_elements;
		for (size_t i = 0; i < t1.elements_types.size(); ++i) {
			compatible_elements.push_back(get_compatible_type(t1.elements_types[i], t2.elements_types[i]));
			if (compatible_elements[i] == Type::VOID) return Type::VOID;
		}
		return Type(RawType::TUPLE, compatible_elements);
	}
	if (t1.raw_type == RawType::SET || t2.raw_type == RawType::SET) return Type::VOID;

	// BOOL, I32, I64, F32, F64
	if (t1 == Type::I32 && t2 == Type::BOOLEAN) return Type::I32;
	if (t2 == Type::I32 && t1 == Type::BOOLEAN) return Type::I32;
	if (t1 == Type::I64 && t2 == Type::BOOLEAN) return Type::I64;
	if (t2 == Type::I64 && t1 == Type::BOOLEAN) return Type::I64;
	if (t1 == Type::I64 && t2 == Type::I32) return Type::I64;
	if (t2 == Type::I64 && t1 == Type::I32) return Type::I64;
	if (t1 == Type::F64 && t2 == Type::I32) return Type::F64;
	if (t2 == Type::F64 && t1 == Type::I32) return Type::F64;
	if (t1 == Type::F64 && t2 == Type::F32) return Type::F64;
	if (t2 == Type::F64 && t1 == Type::F32) return Type::F64;

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

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			elements_types == type.elements_types &&
			return_types == type.return_types &&
			arguments_types == type.arguments_types;
}

bool Type::operator <(const Type& type) const
{
	if (raw_type != type.raw_type) return raw_type < type.raw_type;
	if (elements_types != type.elements_types) return std::lexicographical_compare(elements_types.begin(), elements_types.end(), type.elements_types.begin(), type.elements_types.end());
	if (return_types != type.return_types) return std::lexicographical_compare(return_types.begin(), return_types.end(), type.return_types.begin(), type.return_types.end());
	return std::lexicographical_compare(arguments_types.begin(), arguments_types.end(), type.arguments_types.begin(), type.arguments_types.end());
}

ostream& operator << (ostream& os, const Type& type) {

	if (type == Type::VOID) {
		return os << "{void}";
	}
	if (type == Type::UNREACHABLE) {
		return os << "{unr}";
	}
	if (type.raw_type == RawType::UNKNOWN && !type.elements_types.empty()) {
		os << "{";
		for (size_t i = 0; i < type.elements_types.size(); ++i) {
			if (i > 0) os << "|";
			os << type.elements_types[i];
		}
		return os << "}";
	}

	os << "{" << type.raw_type.name() << Type::get_nature_symbol(type.raw_type.nature());

	if (type.raw_type == RawType::FUNCTION) {
		os << " (";
		for (size_t t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
		}
		os << ") → " << type.return_type();
	}
	if (type.raw_type == RawType::VEC || type.raw_type == RawType::SET) {
		os << " of " << type.element_type(0);
	}
	if (type.raw_type == RawType::MAP) {
		os << " of " << type.element_type(0) << " → " << type.element_type(1);
	}
	os << "}";
	return os;
}



}
