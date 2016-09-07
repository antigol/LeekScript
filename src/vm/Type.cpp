#include "Type.hpp"
#include <cassert>
#include <algorithm>
#include <set>

using namespace std;

namespace ls {

RawType::RawType(const string& name, const string& classname, const string& jsonname, size_t bytes, jit_type_t jit_type, Nature nature, int id)
	: _name(name), _clazz(classname), _json_name(jsonname), _bytes(bytes), _jit_type(jit_type), _nature(nature), id(id)
{}

const RawType RawType::UNKNOWN    ("?",           "",         "",         0,                nullptr,           Nature::UNKNOWN,  -200);
const RawType RawType::VOID       ("void",        "",         "void",     0,                jit_type_void,     Nature::VOID,     11);
const RawType RawType::UNREACHABLE("unreachable", "",         "",         0,                nullptr,           Nature::VOID,     12);
const RawType RawType::LSVALUE    ("lsvalue",     "",         "",         sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  -100);
const RawType RawType::BOOLEAN    ("bool",        "Boolean",  "boolean",  sizeof (int32_t), jit_type_int,      Nature::VALUE,    0);
const RawType RawType::I32        ("i32",         "Number",   "number",   sizeof (int32_t), jit_type_int,      Nature::VALUE,    1);
const RawType RawType::F64        ("f64",         "Number",   "number",   sizeof (double),  jit_type_float64,  Nature::VALUE,    2);
const RawType RawType::I64        ("i64",         "Number",   "number",   sizeof (int64_t), jit_type_long,     Nature::VALUE,    300);
const RawType RawType::F32        ("f32",         "Number",   "number",   sizeof (float),   jit_type_float32,  Nature::VALUE,    400);
const RawType RawType::VAR        ("var",         "Variable", "variable", sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  5);
const RawType RawType::VEC        ("vec",         "Vec",      "vec",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  6);
const RawType RawType::MAP        ("map",         "Map",      "map",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  7);
const RawType RawType::SET        ("set",         "Set",      "set",      sizeof (void*),   jit_type_void_ptr, Nature::LSVALUE,  8);
const RawType RawType::FUNCTION   ("fn",          "",         "fn",       sizeof (void*),   jit_type_void_ptr, Nature::VALUE,    9);
const RawType RawType::TUPLE      ("tuple",       "Tuple",    "tuple",    0,                nullptr,           Nature::VALUE,    10);

const Type Type::UNKNOWN     (&RawType::UNKNOWN);
const Type Type::LSVALUE     (&RawType::LSVALUE);
const Type Type::VOID        (&RawType::VOID);
const Type Type::UNREACHABLE (&RawType::UNREACHABLE);
const Type Type::VAR         (&RawType::VAR);
const Type Type::BOOLEAN     (&RawType::BOOLEAN);
const Type Type::I32         (&RawType::I32);
const Type Type::I64         (&RawType::I64);
const Type Type::F32         (&RawType::F32);
const Type Type::F64         (&RawType::F64);
const Type Type::VEC         (&RawType::VEC,     { Type::UNKNOWN });
const Type Type::VEC_VAR     (&RawType::VEC,     { Type::VAR });
const Type Type::VEC_I32     (&RawType::VEC,     { Type::I32 });
const Type Type::VEC_F64     (&RawType::VEC,     { Type::F64 });
const Type Type::MAP         (&RawType::MAP,     { Type::UNKNOWN, Type::UNKNOWN });
const Type Type::SET         (&RawType::SET,     { Type::UNKNOWN });
const Type Type::FUNCTION    (&RawType::FUNCTION);
const Type Type::TUPLE       (&RawType::TUPLE);
const Type Type::ARITHMETIC  ({ Type::VAR, Type::BOOLEAN, Type::I32, /*Type::I64, Type::F32, */Type::F64 });
const Type Type::LOGIC       ({ Type::LSVALUE, Type::ARITHMETIC });
const Type Type::INDEXABLE   ({ Type::VEC, Type::MAP });
const Type Type::VALUE_NUMBER({ Type::BOOLEAN, Type::I32, /*Type::I64, Type::F32, */Type::F64 });

bool RawType::operator ==(const RawType& type) const {
	return id == type.id;
}

bool RawType::operator <(const RawType& type) const
{
	return id < type.id;
}


string RawType::get_nature_name(const Nature& nature) {
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

string RawType::get_nature_symbol(const Nature& nature) {
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

Type::Type() :
	raw_type(&RawType::UNKNOWN), ph(0)
{}

Type::Type(const RawType* raw_type) :
	raw_type(raw_type), ph(0)
{}

Type::Type(std::initializer_list<Type> alternative_types)
{
	*this = union_iter(alternative_types.begin(), alternative_types.end());
}

Type::Type(const RawType* raw_type, const vector<Type>& elements_types) :
	raw_type(raw_type), ph(0), elements_types(elements_types)
{
	assert(raw_type != &RawType::UNKNOWN);
}

bool Type::must_manage_memory() const {
	assert(is_pure());
	return raw_type->nature() == Nature::LSVALUE;
}

size_t Type::bytes() const
{
	assert(is_pure());
	if (raw_type->bytes() > 0) return raw_type->bytes();
	if (raw_type == &RawType::TUPLE) {
		size_t sum = 0;
		for (const Type& type : elements_types) {
			sum += type.bytes();
		}
		return sum;
	}
	return 0;
}

jit_type_t Type::jit_type() const
{
	assert(is_pure());
	if (raw_type->jit_type() != nullptr) return raw_type->jit_type();
	if (raw_type == &RawType::TUPLE) {
		vector<jit_type_t> fields;
		for (const Type& type : elements_types) {
			fields.push_back(type.jit_type());
		}
		return jit_type_create_struct(fields.data(), fields.size(), 0);
	}
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

void Type::set_return_type(const Type& type) {
	// TODO unknown proof
	assert(raw_type == &RawType::FUNCTION);
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::add_argument_type(const Type& type) {
	// TODO unknown proof
	assert(raw_type == &RawType::FUNCTION);
	arguments_types.push_back(type);
}

void Type::set_argument_type(size_t index, const Type& type) {
	// TODO unknown proof
	assert(raw_type == &RawType::FUNCTION);
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
}

void Type::set_element_type(size_t index, const Type& type) {
	// TODO unknown proof
	assert(raw_type != &RawType::UNKNOWN);
	while (elements_types.size() <= index) {
		elements_types.push_back(Type::UNKNOWN);
	}
	elements_types[index] = type;
}

/*
 *  up to here methods must manage generics types
 */

bool Type::is_pure() const
{
	if (raw_type == &RawType::UNKNOWN || raw_type == &RawType::LSVALUE) return false;
	if (raw_type == &RawType::FUNCTION && return_types.empty()) return false;
	for (const Type& x : elements_types)  if (!x.is_pure()) return false;
	for (const Type& x : return_types)    if (!x.is_pure()) return false;
	for (const Type& x : arguments_types) if (!x.is_pure()) return false;
	return true;
}

void Type::make_it_pure()
{
	if (raw_type == &RawType::UNKNOWN) {
		if (alternative_types.empty()) {
			*this = Type::BOOLEAN;
		} else {
			Type tmp = *alternative_types.begin();
			*this = tmp;
			make_it_pure();
		}
		return;
	}
	if (raw_type == &RawType::LSVALUE) {
		*this = Type::VAR; // smallset id of lsvalue rawtypes
		return;
	}
	if (raw_type == &RawType::FUNCTION && return_types.empty()) {
		return_types.push_back(Type::VOID); // fn()
		return;
	}
	for (Type& x : elements_types)  x.make_it_pure();
	for (Type& x : return_types)    x.make_it_pure();
	for (Type& x : arguments_types) x.make_it_pure();

	assert(is_pure());
}

Type Type::placeholder(int id) const
{
	Type type = *this;
	type.ph = id;
	return type;
}

const RawType* Type::get_raw_type() const
{
	if (raw_type != &RawType::UNKNOWN) return raw_type;
	if (alternative_types.empty()) return &RawType::UNKNOWN;
	auto it = alternative_types.begin();
	const RawType* raw = it->get_raw_type();
	for (++it; it != alternative_types.end(); ++it) {
		const RawType* rawi = it->get_raw_type();
		if (raw != rawi) return &RawType::UNKNOWN;
	}
	return raw;
}

Type Type::return_type() const {
	if (raw_type != &RawType::UNKNOWN) {
		if (0 < return_types.size()) {
			return return_types[0];
		}
		return Type::UNKNOWN;
	}
	if (get_raw_type() != &RawType::UNKNOWN) {
		set<Type> result;
		for (const Type& type : alternative_types) {
			const Type& t = type.return_type();
			if (t.raw_type == &RawType::UNKNOWN) {
				if (t.alternative_types.empty()) return Type::UNKNOWN;
				result.insert(t.alternative_types.begin(), t.alternative_types.end());
			} else {
				result.insert(t);
			}
		}
		return union_iter(result.begin(), result.end());
	}
	return Type::UNKNOWN;
}

Type Type::argument_type(size_t i) const
{
	if (raw_type != &RawType::UNKNOWN) {
		if (i < arguments_types.size()) {
			return arguments_types[i];
		}
		return Type::UNKNOWN;
	}
	if (get_raw_type() != &RawType::UNKNOWN) {
		set<Type> result;
		for (const Type& type : alternative_types) {
			const Type& t = type.argument_type(i);
			if (t.raw_type == &RawType::UNKNOWN) {
				if (t.alternative_types.empty()) return Type::UNKNOWN;
				result.insert(t.alternative_types.begin(), t.alternative_types.end());
			} else {
				result.insert(t);
			}
		}
		return union_iter(result.begin(), result.end());
	}
	return Type::UNKNOWN;
}

Type Type::element_type(size_t i) const {
	if (raw_type != &RawType::UNKNOWN) {
		if (i < elements_types.size()) {
			return elements_types[i];
		}
		return Type::UNKNOWN;
	}
	if (get_raw_type() != &RawType::UNKNOWN) {
		set<Type> result;
		for (const Type& type : alternative_types) {
			const Type& t = type.element_type(i);
			if (t.raw_type == &RawType::UNKNOWN) {
				if (t.alternative_types.empty()) return Type::UNKNOWN;
				result.insert(t.alternative_types.begin(), t.alternative_types.end());
			} else {
				result.insert(t);
			}
		}
		return union_iter(result.begin(), result.end());
	}
	return Type::UNKNOWN;
}

Type Type::image_conversion() const
{
	// return a generic type of all the possible convertion of any alternative of this
	// All these convertions should be implemanted in Compiler::compile_convert
	if (raw_type == &RawType::UNKNOWN) {
		set<Type> result;
		for (const Type& type : alternative_types) {
			Type conv = type.image_conversion();
			if (conv.raw_type == &RawType::UNKNOWN) {
				if (conv.alternative_types.empty()) return Type::UNKNOWN;
				result.insert(conv.alternative_types.begin(), conv.alternative_types.end());
			} else {
				result.insert(conv);
			}
		}
		return union_iter(result.begin(), result.end());
	}

	if (*this == Type::BOOLEAN) return Type({ Type::BOOLEAN, Type::I32, Type::VAR });
	if (*this == Type::I32) return Type({ Type::I32, Type::F64, Type::VAR });
	if (*this == Type::F64) return Type({ Type::F64, Type::VAR });

	// TODO add other possibilities here and in Compiler::compile_convert

	return *this;
}

Type Type::fiber_conversion() const
{
	// return a generic type of all the possible type that can be convert into this (or at least an alternative of this)
	// All these convertions should be implemanted in Compiler::compile_convert
	if (raw_type == &RawType::UNKNOWN) {
		set<Type> result;
		for (const Type& type : alternative_types) {
			Type conv = type.fiber_conversion();
			if (conv.raw_type == &RawType::UNKNOWN) {
				if (conv.alternative_types.empty()) return Type::UNKNOWN;
				result.insert(conv.alternative_types.begin(), conv.alternative_types.end());
			} else {
				result.insert(conv);
			}
		}
		return union_iter(result.begin(), result.end());
	}

	if (*this == Type::LSVALUE) return Type({ Type::BOOLEAN, Type::I32, Type::F64, Type::LSVALUE });
	if (*this == Type::VAR) return Type({ Type::BOOLEAN, Type::I32, Type::F64, Type::VAR });
	if (*this == Type::F64) return Type({ Type::BOOLEAN, Type::I32, Type::F64 });
	if (*this == Type::I32) return Type({ Type::BOOLEAN, Type::I32 });

	// TODO add other possibilities here and in Compiler::compile_convert

	return *this;
}

bool Type::intersection(const Type& t1, const Type& t2, Type* result)
{
	/* UNKNOWN with no alternative is the set of all types (infinite set)
	 * UNKNOWN with alternative is a set of elements
	 * LSVALUE is the set of types of nature lsvalue (infinite set)
	 * All the other types are considered as signlet
	 *
	 * no place holders => cartesian product between sets (elements, arguments, ...)
	 * with place holders => one by one product between sets (elements, arguments, ...)
	 *
	 * empty set => false
	 * non empty set => true
	 */

	Type r;
	bool b;

	if (t1.is_placeholder_free() && t2.is_placeholder_free()) {
		b = priv_intersection_phfree(&t1, &t2, &r) > 0;
	} else {
		Type f1 = t1;
		Type f2 = t2;

		// Avoid place holders collisions
		uint32_t i = 1;
		for (uint32_t old : f1.place_holder_set()) {
			f1.replace_place_holder_id(old, i++);
		}
		for (uint32_t old : f2.place_holder_set()) {
			f2.replace_place_holder_id(old, i++);
		}

		b = priv_intersection_ph(&f1, f1, &f2, f2, &r, r) > 0;
	}

//	if (b) cout << "INTER " << t1 << " <<>> " << t2 << " = " << r << endl;

	if (result) {
		if (b) {
			*result = r;
		}
	}
	return b;
}

bool Type::is_placeholder_free() const
{
	if (ph > 0) return false;
	for (const Type& x : elements_types) {
		if (!x.is_placeholder_free()) return false;
	}
	for (const Type& x : return_types) {
		if (!x.is_placeholder_free()) return false;
	}
	for (const Type& x : arguments_types) {
		if (!x.is_placeholder_free()) return false;
	}
	return true;
}

void Type::replace_place_holder_type(uint32_t id, const Type& type)
{
	if (id == 0) return;
	if (ph == id) {
		*this = type;
		if (!is_pure()) ph = id;
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

Type* Type::copy_iterator(const Type* type, const Type* it)
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

int Type::priv_intersection_ph(Type* t1, Type& f1, Type* t2, Type& f2, Type* tr, Type& fr)
{
	if (t1->raw_type == &RawType::UNKNOWN && !t1->alternative_types.empty()) {

		if (t1->is_placeholder_free() && t2->is_placeholder_free()) {
			return priv_intersection_phfree(t1, t2, tr);
		}

		set<Type> results;
		for (const Type& alternative : t1->alternative_types) {
			Type cf1 = f1;
			Type* ct1 = cf1.copy_iterator(&f1, t1);
			Type cf2 = f2;

			// replace ct1 by its ith element
			if (t1->ph == 0) {
				*ct1 = alternative;
			} else {
				cf1.replace_place_holder_type(t1->ph, alternative);
			}

			Type r;
			if (priv_intersection_ph(&cf1, cf1, &cf2, cf2, &r, r) > 0) {
				if (r.raw_type == &RawType::UNKNOWN) {
					if (r.alternative_types.empty()) {
						fr = Type::UNKNOWN;
						return 2;
					}
					results.insert(r.alternative_types.begin(), r.alternative_types.end());
				} else {
					results.insert(r);
				}
			}
		}
		if (results.empty()) return 0;
		else if (results.size() == 1) {
			fr = *results.begin();
		} else {
			fr = union_iter(results.begin(), results.end());
		}
		return 2; // the very end
	}
	if (t2->raw_type == &RawType::UNKNOWN && !t2->alternative_types.empty()) {
		return priv_intersection_ph(t2, f2, t1, f1, tr, fr);
	}

	if (t1->raw_type == &RawType::UNKNOWN) {
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
	if (t2->raw_type == &RawType::UNKNOWN) {
		return priv_intersection_ph(t2, f2, t1, f1, tr, fr);
	}

	if (t1->raw_type == &RawType::LSVALUE) {
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
	if (t2->raw_type == &RawType::LSVALUE) {
		return priv_intersection_ph(t2, f2, t1, f1, tr, fr);
	}

	if (t1->raw_type == &RawType::FUNCTION && t1->return_types.empty()) {
		if (t2->raw_type == &RawType::FUNCTION) {
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
	if (t2->raw_type == &RawType::FUNCTION && t2->return_types.empty()) {
		return priv_intersection_ph(t2, f2, t1, f1, tr, fr);
	}

	if (t1->raw_type != t2->raw_type) return 0;
	tr->raw_type = t1->raw_type;

	if (t1->elements_types.size() != t2->elements_types.size()) return 0;
	tr->elements_types.resize(t1->elements_types.size());
	for (size_t i = 0; i < t1->elements_types.size(); ++i) {
		int res = priv_intersection_ph(&t1->elements_types[i], f1, &t2->elements_types[i], f2, &tr->elements_types[i], fr);
		if (res != 1) return res;
	}

	if (t1->return_types.size() != t2->return_types.size()) return 0;
	tr->return_types.resize(t1->return_types.size());
	for (size_t i = 0; i < t1->return_types.size(); ++i) {
		int res = priv_intersection_ph(&t1->return_types[i], f1, &t2->return_types[i], f2, &tr->return_types[i], fr);
		if (res != 1) return res;
	}

	if (t1->arguments_types.size() != t2->arguments_types.size()) return 0;
	tr->arguments_types.resize(t1->arguments_types.size());
	for (size_t i = 0; i < t1->arguments_types.size(); ++i) {
		int res = priv_intersection_ph(&t1->arguments_types[i], f1, &t2->arguments_types[i], f2, &tr->arguments_types[i], fr);
		if (res != 1) return res;
	}

	uint32_t new_ph = t2->ph > 0 ? t2->ph : t1->ph;
	uint32_t old_ph = t1->ph;
	f1.replace_place_holder_id(old_ph, new_ph);
	f2.replace_place_holder_id(old_ph, new_ph);
	fr.replace_place_holder_id(old_ph, new_ph);

	// update ph types
	f1.replace_place_holder_type(new_ph, *tr);
	f2.replace_place_holder_type(new_ph, *tr);
	fr.replace_place_holder_type(new_ph, *tr);

	return 1;
}

int Type::priv_intersection_phfree(const Type* t1, const Type* t2, Type* tr)
{
	if (t1->raw_type == &RawType::UNKNOWN && !t1->alternative_types.empty()) {
		set<Type> results;
		for (const Type& alternative : t1->alternative_types) {
			Type tmp;
			if (priv_intersection_phfree(&alternative, t2, &tmp) > 0) {
				if (tmp.raw_type == &RawType::UNKNOWN) {
					if (tmp.alternative_types.empty()) {
						*tr = Type::UNKNOWN;
						return 1;
					}
					results.insert(tmp.alternative_types.begin(), tmp.alternative_types.end());
				} else {
					results.insert(tmp);
				}
			}
		}
		if (results.empty()) return 0;
		else if (results.size() == 1) {
			*tr = *results.begin();
		} else {
			*tr = union_iter(results.begin(), results.end());
		}
		return 1;
	}
	if (t2->raw_type == &RawType::UNKNOWN && !t2->alternative_types.empty()) {
		return priv_intersection_phfree(t2, t1, tr);
	}

	if (t1->raw_type == &RawType::UNKNOWN) {
		*tr = *t2;
		return 1;
	}
	if (t2->raw_type == &RawType::UNKNOWN) {
		*tr = *t1;
		return 1;
	}

	if (t1->raw_type == &RawType::LSVALUE) {
		if (t2->raw_type->nature() == Nature::LSVALUE) {
			*tr = *t2;
			return 1;
		}
		return 0;
	}
	if (t2->raw_type == &RawType::LSVALUE) {
		if (t1->raw_type->nature() == Nature::LSVALUE) {
			*tr = *t1;
			return 1;
		}
		return 0;
	}

	if (t1->raw_type == &RawType::FUNCTION && t1->return_types.empty()) {
		if (t2->raw_type == &RawType::FUNCTION) {
			*tr = *t2;
			return 1;
		}
		return 0;
	}
	if (t2->raw_type == &RawType::FUNCTION && t2->return_types.empty()) {
		if (t1->raw_type == &RawType::FUNCTION) {
			*tr = *t1;
			return 1;
		}
		return 0;
	}

	if (t1->raw_type != t2->raw_type) return 0;
	tr->raw_type = t1->raw_type;

	if (t1->elements_types.size() != t2->elements_types.size()) return 0;
	tr->elements_types.resize(t1->elements_types.size());
	for (size_t i = 0; i < t1->elements_types.size(); ++i) {
		int res = priv_intersection_phfree(&t1->elements_types[i], &t2->elements_types[i], &tr->elements_types[i]);
		if (res != 1) return res;
	}

	if (t1->return_types.size() != t2->return_types.size()) return 0;
	tr->return_types.resize(t1->return_types.size());
	for (size_t i = 0; i < t1->return_types.size(); ++i) {
		int res = priv_intersection_phfree(&t1->return_types[i], &t2->return_types[i], &tr->return_types[i]);
		if (res != 1) return res;
	}

	if (t1->arguments_types.size() != t2->arguments_types.size()) return 0;
	tr->arguments_types.resize(t1->arguments_types.size());
	for (size_t i = 0; i < t1->arguments_types.size(); ++i) {
		int res = priv_intersection_phfree(&t1->arguments_types[i], &t2->arguments_types[i], &tr->arguments_types[i]);
		if (res != 1) return res;
	}

	return 1;
}

Type Type::union_of(const Type& t1, const Type& t2)
{
	if (t1.raw_type == &RawType::UNKNOWN && t1.alternative_types.empty()) return Type::UNKNOWN;
	if (t2.raw_type == &RawType::UNKNOWN && t2.alternative_types.empty()) return Type::UNKNOWN;
	if (t1.raw_type == &RawType::UNKNOWN && t2.raw_type == &RawType::UNKNOWN) {
		set<Type> result;
		result.insert(t1.alternative_types.begin(), t1.alternative_types.end());
		result.insert(t2.alternative_types.begin(), t2.alternative_types.end());
		return union_iter(result.begin(), result.end());
	}
	if (t1.raw_type == &RawType::UNKNOWN) {
		Type result = t1;
		result.alternative_types.insert(t2);
		return result;
	}
	if (t2.raw_type == &RawType::UNKNOWN) {
		Type result = t2;
		result.alternative_types.insert(t1);
		return result;
	}
	// l'un des deux est un generic de lsvalue
	if (t1.raw_type == &RawType::LSVALUE && t2.raw_type->nature() == Nature::LSVALUE) return Type::LSVALUE;
	if (t2.raw_type == &RawType::LSVALUE && t1.raw_type->nature() == Nature::LSVALUE) return Type::LSVALUE;

	// l'un des deux est un generic de fonction
	if (t1.raw_type == &RawType::FUNCTION && t1.return_types.empty() && t2.raw_type == &RawType::FUNCTION) return Type::FUNCTION;
	if (t2.raw_type == &RawType::FUNCTION && t2.return_types.empty() && t1.raw_type == &RawType::FUNCTION) return Type::FUNCTION;


	// Example : map<i32,f64> union map<f64,i32>  !=  map<{i32|f64},{i32|f64}>
	// Example : map<i32,f64> union map<i32,i32>  ==  map<i32,{i32|f64}>     TODO

	// si on NE peut PAS merger les sous parties
	int in1 = t1.elements_types.size() + t1.return_types.size() + t1.arguments_types.size();
	int in2 = t2.elements_types.size() + t2.return_types.size() + t1.arguments_types.size();
	if (t1.raw_type != t2.raw_type
			|| in1 > 1 || in2 > 1
			|| t1.elements_types.size() != t2.elements_types.size()
			|| t1.return_types.size() != t2.return_types.size()
			|| t1.arguments_types.size() != t2.arguments_types.size()) {
		Type result = Type::UNKNOWN;
		result.alternative_types.insert(t1);
		result.alternative_types.insert(t2);
		return result;
	}

	// si on peut merger les sous parties
	Type result(t1.raw_type);
	result.elements_types.reserve(t1.elements_types.size());
	result.return_types.reserve(t1.return_types.size());
	result.arguments_types.reserve(t1.arguments_types.size());

	for (size_t i = 0; i < t1.elements_types.size(); ++i) {
		result.elements_types.push_back(union_of(t1.elements_types[i], t2.elements_types[i]));
	}
	for (size_t i = 0; i < t1.return_types.size(); ++i) {
		result.return_types.push_back(union_of(t1.return_types[i], t2.return_types[i]));
	}
	for (size_t i = 0; i < t1.arguments_types.size(); ++i) {
		result.arguments_types.push_back(union_of(t1.arguments_types[i], t2.arguments_types[i]));
	}
	return result;
}

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			alternative_types == type.alternative_types &&
			elements_types == type.elements_types &&
			return_types == type.return_types &&
			arguments_types == type.arguments_types &&
			ph == type.ph;
}

bool Type::operator <(const Type& type) const
{
	if (*raw_type != *type.raw_type) return *raw_type < *type.raw_type;
	if (alternative_types != type.alternative_types) return std::lexicographical_compare(alternative_types.begin(), alternative_types.end(), type.alternative_types.begin(), type.alternative_types.end());
	if (elements_types != type.elements_types) return std::lexicographical_compare(elements_types.begin(), elements_types.end(), type.elements_types.begin(), type.elements_types.end());
	if (return_types != type.return_types) return std::lexicographical_compare(return_types.begin(), return_types.end(), type.return_types.begin(), type.return_types.end());
	if (arguments_types != type.arguments_types) return std::lexicographical_compare(arguments_types.begin(), arguments_types.end(), type.arguments_types.begin(), type.arguments_types.end());
	return ph < type.ph;
}

ostream& operator << (ostream& os, const Type& type) {

	if (type == Type::VOID) {
		return os << "{void}" << flush;
	}
	if (type == Type::UNREACHABLE) {
		return os << "{unr}" << flush;
	}
	if (type.raw_type == &RawType::UNKNOWN && !type.alternative_types.empty()) {
		os << "{";
		for (auto it = type.alternative_types.begin(); it != type.alternative_types.end(); ++it) {
			if (it != type.alternative_types.begin()) os << "|";
			os << *it;
		}
		os << "}";
		if (type.ph > 0) os << "_" << type.ph;
		return os << flush;
	}

	os << "{" << type.raw_type->name() << RawType::get_nature_symbol(type.raw_type->nature()) << flush;

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
	if (type.ph > 0) os << "_" << type.ph;
	return os << flush;
}



}
