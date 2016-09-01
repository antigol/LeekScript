#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>

namespace ls {

enum class Nature {
	UNKNOWN, VALUE, LSVALUE, VOID
};

class RawType {
private:
	std::string _name;
	std::string _classname;
	std::string _jsonname;

	RawType(const std::string& name, const std::string& classname, const std::string& jsonname);

public:
	const std::string getName()     const { return _name; }
	const std::string getClass()    const { return _classname; }
	const std::string getJsonName() const { return _jsonname; }

	static const RawType UNKNOWN;
	static const RawType VOID;
	static const RawType UNREACHABLE;
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

class Type {
public:

	RawType raw_type;
	Nature nature;
	std::string clazz;
	std::vector<Type> element_types;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;

	Type();
	Type(const RawType& raw_type, Nature nature);
	Type(const RawType& raw_type, Nature nature, const std::vector<Type>& element_types);

	bool must_manage_memory() const;

	Type getReturnType() const;
	void setReturnType(const Type& type);

	void addArgumentType(const Type& type);
	void setArgumentType(size_t index, const Type& type);
	const Type& getArgumentType(size_t index) const;
	const std::vector<Type>& getArgumentTypes() const;

	const Type& getElementType(size_t i = 0) const;
	void setElementType(size_t index, const Type&);

	Type mix(const Type& x) const;
	bool can_be_convert_in(const Type& type) const;
	bool is_primitive_number() const;

	void toJson(std::ostream&) const;

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }


	/*
	 * Static part
	 */
	static const Type UNKNOWN;
	static const Type VOID; // for part of code that returns nothing
	static const Type UNREACHABLE; // for part of code that can't be reached

	static const Type VAR;
	static const Type BOOLEAN;
	static const Type I32;
	static const Type I64;
	static const Type F32;
	static const Type F64;
	static const Type VEC;
	static const Type MAP;
	static const Type SET;
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
