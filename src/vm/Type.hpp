#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>
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

	RawType(const std::string& name, const std::string& classname, const std::string& jsonname, size_t bytes, jit_type_t jit_type, Nature nature);

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
};

// null < bool,number < text < vec < map < set < function < tuple


class Type {
public:

	RawType raw_type;
	std::string clazz;
	std::vector<Type> elements_types;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;
	int ph; // not compared in ==

	Type();
	Type(const RawType& raw_type);
	Type(const RawType& raw_type, const std::vector<Type>& elements_types);

	Type place_holder(int id) const;
	bool must_manage_memory() const;

	Type return_type() const;
	void set_return_type(const Type& type);

	void add_argument_type(const Type& type);
	void set_argument_type(size_t index, const Type& type);
	const Type& argument_type(size_t index) const;

	const Type& element_type(size_t i) const;
	void set_element_type(size_t index, const Type&);

	bool can_be_convert_in(const Type& type) const;
	bool is_primitive_number() const;
	bool is_arithmetic() const;

	void replace_place_holder(int id, const Type& type);
	Type match_with_generic(const Type& generic) const;
private:
	bool match_with_generic_private(const Type& generic, Type& complete) const;
public:

	void toJson(std::ostream&) const;

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }


	/*
	 * Static part
	 */
	static const Type UNKNOWN;
	static const Type VOID; // for part of code that returns nothing
	static const Type UNREACHABLE; // for part of code that can't be reached

	static const Type LSVALUE; // UNKNOWN LSVALUE

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
	static const Type FUNCTION;
	static const Type TUPLE;

	static bool list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww);
	static bool more_specific(const Type& old, const Type& neww);
	static Type get_compatible_type(const Type& t1, const Type& t2);
	static std::string get_nature_name(const Nature& nature);
	static std::string get_nature_symbol(const Nature& nature);
};

std::ostream& operator << (std::ostream&, const Type&);

}

#endif
