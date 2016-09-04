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
const RawType RawType::LSVALUE    ("lsvalue",     "?",        "?",        sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  14);
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

const Type Type::UNKNOWN    (&RawType::UNKNOWN);
const Type Type::LSVALUE    (&RawType::LSVALUE);
const Type Type::VOID       (&RawType::VOID);
const Type Type::UNREACHABLE(&RawType::UNREACHABLE);
const Type Type::VAR        (&RawType::VAR);
const Type Type::BOOLEAN    (&RawType::BOOLEAN);
const Type Type::I32        (&RawType::I32);
const Type Type::I64        (&RawType::I64);
const Type Type::F32        (&RawType::F32);
const Type Type::F64        (&RawType::F64);
const Type Type::VEC        (&RawType::VEC, { Type::UNKNOWN });
const Type Type::VEC_VAR    (&RawType::VEC, { Type::VAR });
const Type Type::VEC_I32    (&RawType::VEC, { Type::I32 });
const Type Type::VEC_F64    (&RawType::VEC, { Type::F64 });
const Type Type::MAP        (&RawType::VEC, { Type::UNKNOWN, Type::UNKNOWN });
const Type Type::SET        (&RawType::VEC, { Type::UNKNOWN });
const Type Type::FUNCTION   (&RawType::FUNCTION);
const Type Type::TUPLE      (&RawType::TUPLE);

Type::Type() :
	raw_type(&RawType::UNKNOWN), ph(0)
{}

Type::Type(const RawType* raw_type) :
	raw_type(raw_type), ph(0)
{}

Type::Type(const RawType* raw_type, const vector<Type>& elements_types) :
	raw_type(raw_type), elements_types(elements_types), ph(0)
{}

Type Type::place_holder(int id) const
{
	Type type = *this;
	type.ph = id;
	return type;
}

bool Type::must_manage_memory() const {
	return raw_type->nature() == Nature::LSVALUE;
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

bool Type::can_be_convert_in(const Type& type) const {
	return get_compatible_type(*this, type) == type;
}

bool Type::is_primitive_number() const
{
	if (raw_type == &RawType::UNKNOWN) {
		for (const Type& type : elements_types) if (type.is_primitive_number()) return true;
	}
	return *this == Type::BOOLEAN || *this == Type::I32 || *this == Type::I64 || *this == Type::F32 || *this == Type::F64;
}

bool Type::is_arithmetic() const
{
	if (raw_type == &RawType::UNKNOWN) {
		for (const Type& type : elements_types) if (type.is_arithmetic()) return true;
	}
	return *this == Type::VAR || *this == Type::BOOLEAN || *this == Type::I32 || *this == Type::I64 || *this == Type::F32 || *this == Type::F64;
}

bool Type::is_complete() const
{
	if (raw_type == &RawType::UNKNOWN || raw_type == &RawType::LSVALUE) return false;
	for (const Type& x : elements_types)  if (!x.is_complete()) return false;
	for (const Type& x : return_types)    if (!x.is_complete()) return false;
	for (const Type& x : arguments_types) if (!x.is_complete()) return false;
	return true;
}

void Type::make_it_complete()
{
	if (raw_type == &RawType::UNKNOWN) {
		if (elements_types.empty()) {
			*this = Type::I32;
		} else {
			*this = elements_types[0];
			make_it_complete();
		}
		return;
	}
	if (*this == Type::LSVALUE) {
		*this = Type::VAR;
		return;
	}
	for (Type& x : elements_types)  x.make_it_complete();
	for (Type& x : return_types)    x.make_it_complete();
	for (Type& x : arguments_types) x.make_it_complete();

	assert(is_complete());
}

void Type::replace_place_holder_type(uint32_t id, const Type& type)
{
	if (id == 0) return;
	if (ph == id) {
		*this = type;
		ph = id;
	}
	for (Type& x : elements_types)  x.replace_place_holder_type(id, type);
	for (Type& x : return_types)    x.replace_place_holder_type(id, type);
	for (Type& x : arguments_types) x.replace_place_holder_type(id, type);
}

void Type::replace_place_holder_id(uint32_t old_id, uint32_t new_id)
{
	if (old_id == 0 || new_id == 0) return;
	if (ph == old_id) ph = new_id;
	for (Type& x : elements_types)  x.replace_place_holder_id(old_id, new_id);
	for (Type& x : return_types)    x.replace_place_holder_id(old_id, new_id);
	for (Type& x : arguments_types) x.replace_place_holder_id(old_id, new_id);
}

set<uint32_t> Type::place_holder_set() const
{
	set<uint32_t> phs;
	if (ph > 0) phs.insert(ph);
	for (const Type& x : elements_types) {
		set<uint32_t> sub = x.place_holder_set();
		phs.insert(sub.begin(), sub.end());
	}
	for (const Type& x : return_types) {
		set<uint32_t> sub = x.place_holder_set();
		phs.insert(sub.begin(), sub.end());
	}
	for (const Type& x : arguments_types) {
		set<uint32_t> sub = x.place_holder_set();
		phs.insert(sub.begin(), sub.end());
	}
	return phs;
}

void Type::clean_place_holders()
{
	if (raw_type != &RawType::UNKNOWN && raw_type != &RawType::LSVALUE) ph = 0;
	for (Type& x : elements_types)  x.clean_place_holders();
	for (Type& x : return_types)    x.clean_place_holders();
	for (Type& x : arguments_types) x.clean_place_holders();
}

jit_type_t Type::jit_type() const
{
	if (raw_type->jit_type() != nullptr) return raw_type->jit_type();
	if (raw_type == &RawType::TUPLE) {
		vector<jit_type_t> fields;
		for (const Type& type : elements_types) {
			fields.push_back(type.jit_type());
		}
		return jit_type_create_struct(fields.data(), fields.size(), 0);
	}
	assert(0);
	return nullptr;
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type->json_name() << "\"";

	if (raw_type == &RawType::FUNCTION) {
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

bool Type::get_intersection(const Type& t1, const Type& t2, Type* result)
{
	/* UNKNOWN with no elements is the set of all types (infinite set)
	 * UNKNOWN with elements is a set of elements
	 * LSVALUE is the set of types of nature lsvalue (infinite set)
	 * All the other types are considered as signlet
	 *
	 * no place holders => cartesian product between sets (elements, arguments, ...)
	 * with place holders => one by one product between sets (elements, arguments, ...)
	 *
	 * empty set => false
	 * non empty set => true
	 */
	Type f1 = t1;
	Type f2 = t2;

	// Avoid place holders collisions
	set<uint32_t> phs1 = f1.place_holder_set();
	set<uint32_t> phs2 = f2.place_holder_set();
	uint32_t i = 1;
	for (auto it = phs1.begin(); it != phs1.end(); ++it) {
		f1.replace_place_holder_id(*it, i++);
	}
	for (auto it = phs2.begin(); it != phs2.end(); ++it) {
		f2.replace_place_holder_id(*it, i++);
	}

	Type r;
	bool b = get_intersection_private(&f1, &f2, f1, f2, &r, r) > 0;
	if (result) {
		r.clean_place_holders();
		*result = r;
	}
	return b;
}

Type* Type::copy_iterator(Type* type, Type* it)
{
	if (it == type) return this;
	for (size_t i = 0; i < elements_types.size(); ++i) {
		Type* r = elements_types[i].copy_iterator(&type->elements_types[i], it);
		if (r) return r;
	}
	for (size_t i = 0; i < return_types.size(); ++i) {
		Type* r = return_types[i].copy_iterator(&type->return_types[i], it);
		if (r) return r;
	}
	for (size_t i = 0; i < arguments_types.size(); ++i) {
		Type* r = arguments_types[i].copy_iterator(&type->arguments_types[i], it);
		if (r) return r;
	}
	return nullptr;
}

int Type::get_intersection_private(Type* t1, Type* t2, Type& f1, Type& f2, Type* tr, Type& fr)
{
	if (t1->raw_type == &RawType::UNKNOWN && !t1->elements_types.empty()) {
		vector<Type> elements_types;
		for (size_t i = 0; i < t1->elements_types.size(); ++i) {
			Type cf1 = f1;
			Type* ct1 = cf1.copy_iterator(&f1, t1);
			Type cf2 = f2;

			// replace ct1 by its ith element
			cf1.replace_place_holder_type(ct1->ph, t1->elements_types[i]);
			*ct1 = t1->elements_types[i];

			Type r;
			if (get_intersection_private(&cf1, &cf2, cf1, cf2, &r, r) > 0) {
				if (r == Type::UNKNOWN) {
					fr = Type::UNKNOWN;
					return 2;
				}
				if (r.raw_type == &RawType::UNKNOWN) {
					elements_types.insert(elements_types.end(), r.elements_types.begin(), r.elements_types.end());
				} else {
					elements_types.push_back(r);
				}
			}
		}
		if (elements_types.empty()) return false;
		else if (elements_types.size() == 1) {
			fr = elements_types[0];
		} else {
			fr = Type(&RawType::UNKNOWN, elements_types);
		}
		return 2; // the very end
	}
	if (t2->raw_type == &RawType::UNKNOWN && !t2->elements_types.empty()) {
		return get_intersection_private(t2, t1, f2, f1, tr, fr);
	}

	if (*t1 == Type::UNKNOWN) {
		*tr = *t2;
		if (t1->ph > 0) tr->ph = t1->ph;

		// in case t1 && t2 are both ph > 0
		uint32_t old_ph = t2->ph;
		uint32_t new_ph = tr->ph;
		f1.replace_place_holder_id(old_ph, new_ph);
		f2.replace_place_holder_id(old_ph, new_ph);
		fr.replace_place_holder_id(old_ph, new_ph);

		// update ph types
		f1.replace_place_holder_type(new_ph, *t2);
		f2.replace_place_holder_type(new_ph, *t2);
		fr.replace_place_holder_type(new_ph, *t2);
		return 1;
	}
	if (*t2 == Type::UNKNOWN) {
		return get_intersection_private(t2, t1, f2, f1, tr, fr);
	}

	if (*t1 == Type::LSVALUE) {
		if (t2->raw_type->nature() == Nature::LSVALUE) {
			*tr = *t2;
			if (t1->ph > 0) tr->ph = t1->ph;

			// in case t1 && t2 are both ph > 0
			uint32_t old_ph = t2->ph;
			uint32_t new_ph = tr->ph;
			f1.replace_place_holder_id(old_ph, new_ph);
			f2.replace_place_holder_id(old_ph, new_ph);
			fr.replace_place_holder_id(old_ph, new_ph);

			// update ph types
			f1.replace_place_holder_type(new_ph, *t2);
			f2.replace_place_holder_type(new_ph, *t2);
			fr.replace_place_holder_type(new_ph, *t2);
			return 1;
		}
		return 0;
	}
	if (*t2 == Type::LSVALUE) {
		return get_intersection_private(t2, t1, f2, f1, tr, fr);
	}

	if (t1->raw_type != t2->raw_type) return 0;
	tr->raw_type = t1->raw_type;

	if (t1->elements_types.size() != t2->elements_types.size()) return 0;
	tr->elements_types.resize(t1->elements_types.size());
	for (size_t i = 0; i < t1->elements_types.size(); ++i) {
		int res = get_intersection_private(&t1->elements_types[i], &t2->elements_types[i], f1, f2, &tr->elements_types[i], fr);
		if (res != 1) return res;
	}

	if (t1->return_types.size() != t2->return_types.size()) return 0;
	tr->return_types.resize(t1->return_types.size());
	for (size_t i = 0; i < t1->return_types.size(); ++i) {
		int res = get_intersection_private(&t1->return_types[i], &t2->return_types[i], f1, f2, &tr->return_types[i], fr);
		if (res != 1) return res;
	}

	if (t1->arguments_types.size() != t2->arguments_types.size()) return 0;
	tr->arguments_types.resize(t1->arguments_types.size());
	for (size_t i = 0; i < t1->arguments_types.size(); ++i) {
		int res = get_intersection_private(&t1->arguments_types[i], &t2->arguments_types[i], f1, f2, &tr->arguments_types[i], fr);
		if (res != 1) return res;
	}

	return 1;
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {
	/* Returns a type into both t1 and t2 can be converted
	 */

	if (t1 == t2) {
		return t1;
	}

	if (t1 == Type::UNKNOWN) return t2;
	if (t2 == Type::UNKNOWN) return t1;

	if (t1.raw_type == &RawType::UNKNOWN) {
		set<Type> compatibles;
		for (const Type& type : t1.elements_types) {
			Type compatible = get_compatible_type(t2, type);
			if (compatible.raw_type == &RawType::UNKNOWN) {
				compatibles.insert(compatible.elements_types.begin(), compatible.elements_types.end());
			} else if (compatible != Type::VOID) compatibles.insert(compatible);
		}
		if (compatibles.empty()) return Type::VOID;
		if (compatibles.size() == 1) return *compatibles.begin();
		Type type = Type::UNKNOWN;
		type.elements_types.insert(type.elements_types.begin(), compatibles.begin(), compatibles.end());
		return type;
	}
	if (t2.raw_type == &RawType::UNKNOWN) return get_compatible_type(t2, t1);

	if (t1 == Type::LSVALUE) return t2.raw_type->nature() == Nature::LSVALUE ? t2 : Type::VOID;
	if (t2 == Type::LSVALUE) return t1.raw_type->nature() == Nature::LSVALUE ? t1 : Type::VOID;

	// FUNCTION
	if (t1.raw_type == &RawType::FUNCTION && t2.raw_type == &RawType::FUNCTION) {
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
	if (t1.raw_type == &RawType::FUNCTION || t2.raw_type == &RawType::FUNCTION) return Type::VOID;

	// VAR
	if (t1 == Type::VAR && (t2 == Type::BOOLEAN || t2 == Type::I32 || t2 == Type::I64 || t2 == Type::F32 || t2 == Type::F64)) return Type::VAR;
	if (t2 == Type::VAR && (t1 == Type::BOOLEAN || t1 == Type::I32 || t1 == Type::I64 || t1 == Type::F32 || t1 == Type::F64)) return Type::VAR;
	if (t1 == Type::VAR || t2 == Type::VAR) return Type::VOID;

	// VEC
	if (t1.raw_type == &RawType::VEC && t2.raw_type == &RawType::VEC) {
		Type compatible_element = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(&RawType::VEC, { compatible_element });
	}
	if (t1.raw_type == &RawType::VEC || t2.raw_type == &RawType::VEC) return Type::VOID;

	// SET
	if (t1.raw_type == &RawType::SET && t2.raw_type == &RawType::SET) {
		Type compatible_element = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_element == Type::VOID) return Type::VOID;
		return Type(&RawType::SET, { compatible_element });
	}
	if (t1.raw_type == &RawType::SET || t2.raw_type == &RawType::SET) return Type::VOID;

	// MAP
	if (t1.raw_type == &RawType::MAP && t2.raw_type == &RawType::MAP) {
		Type compatible_key = get_compatible_type(t1.element_type(0), t2.element_type(0));
		if (compatible_key == Type::VOID) return Type::VOID;
		Type compatible_value = get_compatible_type(t1.element_type(1), t2.element_type(1));
		if (compatible_value == Type::VOID) return Type::VOID;
		return Type(&RawType::MAP, { compatible_key, compatible_value });
	}
	if (t1.raw_type == &RawType::MAP || t2.raw_type == &RawType::MAP) return Type::VOID;

	// TUPLE
	if (t1.raw_type == &RawType::TUPLE && t2.raw_type == &RawType::TUPLE) {
		if (t1.elements_types.size() != t2.elements_types.size()) return Type::VOID;
		vector<Type> compatible_elements;
		for (size_t i = 0; i < t1.elements_types.size(); ++i) {
			compatible_elements.push_back(get_compatible_type(t1.elements_types[i], t2.elements_types[i]));
			if (compatible_elements[i] == Type::VOID) return Type::VOID;
		}
		return Type(&RawType::TUPLE, compatible_elements);
	}
	if (t1.raw_type == &RawType::SET || t2.raw_type == &RawType::SET) return Type::VOID;

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
	if (type.raw_type == &RawType::UNKNOWN && !type.elements_types.empty()) {
		os << "{";
		for (size_t i = 0; i < type.elements_types.size(); ++i) {
			if (i > 0) os << "|";
			os << type.elements_types[i];
		}
		return os << "}";
	}

	os << "{" << type.raw_type->name() << Type::get_nature_symbol(type.raw_type->nature());
	if (type.ph > 0) os << "_" << type.ph;

	if (type.raw_type == &RawType::FUNCTION) {
		os << " (";
		for (size_t t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
		}
		os << ") → " << type.return_type();
	}
	if (type.raw_type == &RawType::VEC || type.raw_type == &RawType::SET) {
		os << " of " << type.element_type(0);
	}
	if (type.raw_type == &RawType::MAP) {
		os << " of " << type.element_type(0) << " → " << type.element_type(1);
	}
	if (type.raw_type == &RawType::TUPLE) {
		os << " of (";
		for (size_t t = 0; t < type.elements_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.elements_types[t];
		}
		os << ")";
	}
	os << "}";
	return os;
}



}
