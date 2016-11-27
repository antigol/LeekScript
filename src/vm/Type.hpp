#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>

namespace ls {

enum class Nature {
	UNKNOWN, VALUE, POINTER, VOID
};

class BaseRawType {
public:
	virtual ~BaseRawType() {}
	virtual const std::string getName() const { return "?"; }
	virtual const std::string getClass() const { return "?"; }
	virtual const std::string getJsonName() const { return "?"; }
};

class VoidRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "void"; }
	virtual const std::string getJsonName() const { return "void"; }
};

class NullRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "null"; }
	virtual const std::string getClass() const { return "Null"; }
	virtual const std::string getJsonName() const { return "null"; }
};

class BooleanRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "bool"; }
	virtual const std::string getClass() const { return "Boolean"; }
	virtual const std::string getJsonName() const { return "boolean"; }
};

class NumberRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "number"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class IntegerRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "int"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class GmpIntRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "gmp_int"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class LongRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "long"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class FloatRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "real"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class StringRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "string"; }
	virtual const std::string getClass() const { return "String"; }
	virtual const std::string getJsonName() const { return "string"; }
};

class ArrayRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "array"; }
	virtual const std::string getClass() const { return "Array"; }
	virtual const std::string getJsonName() const { return "array"; }
};

class MapRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "map"; }
	virtual const std::string getClass() const { return "Map"; }
	virtual const std::string getJsonName() const { return "map"; }
};

class SetRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "set"; }
	virtual const std::string getClass() const { return "Set"; }
	virtual const std::string getJsonName() const { return "set"; }
};

class IntervalRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "interval"; }
	virtual const std::string getClass() const { return "Interval"; }
	virtual const std::string getJsonName() const { return "interval"; }
};

class ObjectRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getClass() const { return "Object"; }
	virtual const std::string getJsonName() const { return "object"; }
};

class FunctionRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getClass() const { return "Function"; }
	virtual const std::string getJsonName() const { return "function"; }
};

class ClassRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "class"; }
	virtual const std::string getClass() const { return "Class"; }
	virtual const std::string getJsonName() const { return "class"; }
};

class RawType {
public:
	static const BaseRawType* const UNKNOWN;
	static const VoidRawType* const VOID;
	static const NullRawType* const NULLL;
	static const BooleanRawType* const BOOLEAN;
	static const NumberRawType* const NUMBER;
	static const GmpIntRawType* const GMP_INT;
	static const IntegerRawType* const UNSIGNED_INTEGER;
	static const IntegerRawType* const INTEGER;
	static const LongRawType* const UNSIGNED_LONG;
	static const LongRawType* const LONG;
	static const FloatRawType* const REAL;
	static const StringRawType* const STRING;
	static const ArrayRawType* const ARRAY;
	static const MapRawType* const MAP;
	static const SetRawType* const SET;
	static const IntervalRawType* const INTERVAL;
	static const ObjectRawType* const OBJECT;
	static const FunctionRawType* const FUNCTION;
	static const ClassRawType* const CLASS;
};

class Type {
public:

	const BaseRawType* raw_type;
	Nature nature;
	bool native; // A C++ object, memory management is done outside the language
	std::string clazz;
	std::vector<Type> element_type;
	std::vector<Type> key_type;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;
	bool temporary = false;

	Type();
	Type(const BaseRawType* raw_type, Nature nature, bool native = false, bool temporary = false);
	Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type, bool native = false);
	Type(const BaseRawType* raw_type, Nature nature, const Type& key_type, const Type& element_type, bool native = false);

	bool must_manage_memory() const;

	Type getReturnType() const;
	void setReturnType(Type type);

	void addArgumentType(Type type);
	void setArgumentType(size_t index, Type type);
	const Type& getArgumentType(size_t index) const;
	const std::vector<Type>& getArgumentTypes() const;

	const Type& getElementType() const;
	void setElementType(const Type&);

	const Type& getKeyType() const;
	void setKeyType(const Type&);

	bool will_take(const std::vector<Type>& args_type);
	bool will_take_element(const Type& arg_type);
	Type mix(const Type& x) const;

	void toJson(std::ostream&) const;
	std::string toString() const;

	bool isNumber() const;

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }

	bool compatible(const Type& type) const;

	/*
	 * Static part
	 */
	static const Type VOID;
	static const Type VALUE;
	static const Type POINTER;

	static const Type UNKNOWN;
	static const Type NULLL;
	static const Type BOOLEAN;
	static const Type BOOLEAN_P;
	static const Type NUMBER_VALUE;
	static const Type NUMBER;
	static const Type INTEGER;
	static const Type UNSIGNED_INTEGER;
	static const Type GMP_INT;
	static const Type GMP_INT_TMP;
	static const Type LONG;
	static const Type UNSIGNED_LONG;
	static const Type REAL;
	static const Type STRING;
	static const Type STRING_TMP;
	static const Type OBJECT;
	static const Type ARRAY;
	static const Type PTR_ARRAY;
	static const Type INT_ARRAY;
	static const Type REAL_ARRAY;
	static const Type STRING_ARRAY;
	static const Type MAP;
	static const Type PTR_PTR_MAP;
	static const Type PTR_INT_MAP;
	static const Type PTR_REAL_MAP;
	static const Type REAL_PTR_MAP;
	static const Type REAL_INT_MAP;
	static const Type REAL_REAL_MAP;
	static const Type INT_PTR_MAP;
	static const Type INT_INT_MAP;
	static const Type INT_REAL_MAP;
	static const Type SET;
	static const Type PTR_SET;
	static const Type INT_SET;
	static const Type REAL_SET;
	static const Type INTERVAL;
	static const Type FUNCTION;
	static const Type FUNCTION_P;
	static const Type CLASS;
	static const Type PTR_ARRAY_ARRAY;
	static const Type REAL_ARRAY_ARRAY;
	static const Type INT_ARRAY_ARRAY;

	static bool list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww);
	static bool more_specific(const Type& old, const Type& neww);
	static Type get_compatible_type(const Type& t1, const Type& t2);
	static std::string get_nature_symbol(const Nature& nature);
};

std::ostream& operator << (std::ostream&, const Type&);
std::ostream& operator << (std::ostream&, const std::vector<Type>&);

}

#endif
