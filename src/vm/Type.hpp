#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <set>
#include <iostream>
#include <initializer_list>
#include <jit/jit.h>

namespace ls {

enum class Nature {
	UNKNOWN, VALUE, LSVALUE, VOID
};

class RawType {
private:
	std::string _name;
	std::string _clazz;
	std::string _json_name;
	size_t _bytes;
	jit_type_t _jit_type;
	Nature _nature;
	int id;

	RawType() = delete;
	RawType(const RawType&) = delete;
	RawType(const std::string& name, const std::string& classname, const std::string& jsonname, size_t bytes, jit_type_t jit_type, Nature nature, int id);

public:
	const std::string name()      const { return _name; }
	const std::string clazz()     const { return _clazz; }
	const std::string json_name() const { return _json_name; }
	size_t bytes()                const { return _bytes; }
	jit_type_t jit_type()         const { return _jit_type; }
	Nature nature()               const { return _nature; }

	static const RawType UNKNOWN;
	static const RawType VOID;
	static const RawType UNREACHABLE;
	static const RawType LSVALUE;
	static const RawType VAR;
	static const RawType BOOLEAN;
	static const RawType I32;
	static const RawType I64;
	static const RawType F32;
	static const RawType F64;
	static const RawType VEC;
	static const RawType MAP;
	static const RawType SET;
	static const RawType FUNCTION;
	static const RawType TUPLE;

	bool operator ==(const RawType& type) const;
	inline bool operator !=(const RawType& type) const { return !(*this == type); }
	bool operator <(const RawType& type) const;

	static std::string get_nature_name(const Nature& nature);
	static std::string get_nature_symbol(const Nature& nature);
};

// null < bool,number < text < vec < map < set < function < tuple


class Type {
public:

	const RawType* raw_type;
	uint32_t ph;

	std::set<Type> alternative_types;
	std::vector<Type> elements_types;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;

	Type();
	explicit Type(const RawType* raw_type);
	Type(std::initializer_list<Type> alternative_types);
	Type(const RawType* raw_type, const std::vector<Type>& elements_types);

	bool must_manage_memory() const;

	size_t bytes() const;
	jit_type_t jit_type() const;
	void toJson(std::ostream&) const;

	void set_return_type(const Type& type);
	void add_argument_type(const Type& type);
	void set_argument_type(size_t index, const Type& type);
	void set_element_type(size_t index, const Type&);

	// ^^^^^ pure only ^^^^^

	// vvvvv generics proof vvvvv

	bool is_pure() const;
	void make_it_pure();

	Type placeholder(int id) const;
	const RawType* get_raw_type() const;

	Type return_type() const;
	Type argument_type(size_t i) const;
	Type element_type(size_t i) const;

	Type image_conversion() const;
	Type fiber_conversion() const;

	static bool intersection(const Type& t1, const Type& t2, Type* result = nullptr);

	bool is_placeholder_free() const;

private:
	void replace_place_holder_type(uint32_t id, const Type& type);
	void replace_place_holder_id(uint32_t old_id, uint32_t new_id);
	std::set<uint32_t> place_holder_set() const;
	Type* copy_iterator(const Type* type, const Type* it);
	static int priv_intersection_ph(Type* t1, Type& f1, Type* t2, Type& f2, Type* tr, Type& fr);
	static int priv_intersection_phfree(const Type* t1, const Type* t2, Type* tr);
public:

	static Type union_of(const Type& t1, const Type& t2);

	template <typename IT>
	static Type union_iter(IT first, IT last);

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }
	bool operator <(const Type& type) const;


	static const Type UNKNOWN; // generic for any type if elements_types is empty, otherwise any of the elements_types
	static const Type LSVALUE; // generic for any lsvalue type

	static const Type VOID; // for part of code that returns nothing
	static const Type UNREACHABLE; // for part of code that can't be reached
	static const Type VAR; // LSVALUE
	static const Type BOOLEAN;
	static const Type I32;
	static const Type I64;
	static const Type F32;
	static const Type F64;
	static const Type VEC; // LSVALUE
	static const Type VEC_VAR; // LSVALUE
	static const Type VEC_I32; // LSVALUE
	static const Type VEC_F64; // LSVALUE
	static const Type MAP; // LSVALUE
	static const Type SET; // LSVALUE
	static const Type FUNCTION; // generic if no return_type
	static const Type TUPLE;

	// Some useful generics
	static const Type ARITHMETIC;
	static const Type LOGIC;
	static const Type INDEXABLE;
	static const Type VALUE_NUMBER;
};

template <typename IT>
Type Type::union_iter(IT first, IT last)
{
	if (first == last) return Type::UNKNOWN;
	Type result = *first;
	++first;

	while (first != last) {
		result = union_of(result, *first);
		++first;
	}

	return result;
}

std::ostream& operator << (std::ostream&, const Type&);

} // ls

#endif
