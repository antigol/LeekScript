#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include <map>

#include "../../lib/json.hpp"
#include "Type.hpp"

namespace ls {

class LSVar;
template <typename T> class LSVec;
template <typename K, typename T> class LSMap;
template <typename T> class LSSet;
class Context;


#define LSVALUE_OPERATORS \
	bool req(const LSValue* value) const override { return value->eq(this); } \
	bool rlt(const LSValue* value) const override { return value->lt(this); } \

class LSValue {
public:

	static int obj_count;
	static int obj_deleted;

	int refs = 0;

	LSValue();
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual std::ostream& print(std::ostream&) const = 0;
	static std::ostream& print(std::ostream& os, const LSValue* value);

	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;

	static LSValue* parse(Json& json);

	static LSValue* clone_inc(LSValue* value);
	static LSValue* move(LSValue* value);
	static LSValue* move_inc(LSValue* value);
	static void delete_ref(LSValue* value);
	static void delete_temporary(LSValue* value);
	static void inc_refs(LSValue* value);

	bool operator == (const LSValue& value) const { return value.req(this); }
	bool operator != (const LSValue& value) const { return !value.req(this); }
	virtual bool req(const LSValue*) const = 0;
	virtual bool eq(const LSVar*) const;
	virtual bool eq(const LSVec<LSValue*>*) const;
	virtual bool eq(const LSVec<void*>*) const;
	virtual bool eq(const LSVec<int32_t>*) const;
	virtual bool eq(const LSVec<int64_t>*) const;
	virtual bool eq(const LSVec<float>*) const;
	virtual bool eq(const LSVec<double>*) const;
	virtual bool eq(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool eq(const LSMap<LSValue*,int>*) const;
	virtual bool eq(const LSMap<LSValue*,double>*) const;
	virtual bool eq(const LSMap<int,LSValue*>*) const;
	virtual bool eq(const LSMap<int,int>*) const;
	virtual bool eq(const LSMap<int,double>*) const;
	virtual bool eq(const LSSet<LSValue*>*) const;
	virtual bool eq(const LSSet<void*>*) const;
	virtual bool eq(const LSSet<int32_t>*) const;
	virtual bool eq(const LSSet<int64_t>*) const;
	virtual bool eq(const LSSet<float>*) const;
	virtual bool eq(const LSSet<double>*) const;

	virtual int typeID() const = 0;

	bool operator < (const LSValue& value) const { return value.rlt(this); }
	bool operator > (const LSValue& value) const { return !(*this == value) && !(*this < value); }
	bool operator <=(const LSValue& value) const { return (*this == value) || (*this < value); }
	bool operator >=(const LSValue& value) const { return !(*this < value); }
	virtual bool rlt(const LSValue*) const = 0;
	virtual bool lt(const LSVar*) const;
	virtual bool lt(const LSVec<LSValue*>*) const;
	virtual bool lt(const LSVec<void*>*) const;
	virtual bool lt(const LSVec<int32_t>*) const;
	virtual bool lt(const LSVec<int64_t>*) const;
	virtual bool lt(const LSVec<float>*) const;
	virtual bool lt(const LSVec<double>*) const;
	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool lt(const LSMap<LSValue*,int>*) const;
	virtual bool lt(const LSMap<LSValue*,double>*) const;
	virtual bool lt(const LSMap<int,LSValue*>*) const;
	virtual bool lt(const LSMap<int,int>*) const;
	virtual bool lt(const LSMap<int,double>*) const;
	virtual bool lt(const LSSet<LSValue*>*) const;
	virtual bool lt(const LSSet<int>*) const;
	virtual bool lt(const LSSet<double>*) const;
};

inline std::ostream& LSValue::print(std::ostream& os, const LSValue* value) {
	if (value == nullptr) return os << "null";
	return value->print(os);
}

inline LSValue* LSValue::clone_inc(LSValue* value) {
	if (value == nullptr) return nullptr;
	LSValue* copy = value->clone();
	copy->refs++;
	return copy;
}

inline LSValue* LSValue::move(LSValue* value) {
	if (value == nullptr) return nullptr;
	if (value->refs == 0) {
		return value;
	}
	return value->clone();
}

inline LSValue* LSValue::move_inc(LSValue* value) {
	if (value == nullptr) return nullptr;

	if (value->refs == 0) {
		value->refs++;
		return value;
	} else {
		LSValue* copy = value->clone();
		copy->refs++;
		return copy;
	}
}

inline void LSValue::delete_ref(LSValue* value) {

	if (value == nullptr) return;
	if (value->refs == 0) return;

	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(LSValue* value) {

	if (value == nullptr) return;

	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::inc_refs(LSValue* value)
{
	if (value == nullptr) return;
	value->refs++;
}

inline std::ostream& operator << (std::ostream& os, const LSValue* value) {
	if (value == nullptr) return os << "null";
	return value->print(os);
}

}

#endif

