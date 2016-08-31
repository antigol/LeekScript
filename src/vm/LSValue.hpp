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
	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual int typeID() const = 0;

	virtual RawType getRawType() const = 0;

	virtual std::string getClass() const;

	static LSValue* parse(Json& json);

	static void delete_ref(LSValue* value);
	static void delete_temporary(LSValue* value);

	bool operator == (const LSValue& value) const { return value.req(this); }
	bool operator != (const LSValue& value) const { return !value.req(this); }
	virtual bool req(const LSValue*) const = 0;
	virtual bool eq(const LSVar*) const;
	virtual bool eq(const LSVec<LSValue*>*) const;
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
	virtual bool eq(const LSSet<int32_t>*) const;
	virtual bool eq(const LSSet<int64_t>*) const;
	virtual bool eq(const LSSet<float>*) const;
	virtual bool eq(const LSSet<double>*) const;

	bool operator < (const LSValue& value) const { return value.rlt(this); }
	bool operator > (const LSValue& value) const { return !(*this == value) && !(*this < value); }
	bool operator <=(const LSValue& value) const { return (*this == value) || (*this < value); }
	bool operator >=(const LSValue& value) const { return !(*this < value); }
	virtual bool rlt(const LSValue*) const = 0;
	virtual bool lt(const LSVar*) const;
	virtual bool lt(const LSVec<LSValue*>*) const;
	virtual bool lt(const LSVec<int>*) const;
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

inline LSValue* LSValue::clone_inc() {
	LSValue* copy = clone();
	copy->refs++;
	return copy;
}

inline LSValue* LSValue::move() {
	if (refs == 0) {
		return this;
	}
	return clone();
}

inline LSValue* LSValue::move_inc() {
	if (refs == 0) {
		refs++;
		return this;
	} else {
		LSValue* copy = clone();
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

inline std::ostream& operator << (std::ostream& os, const LSValue& value) {
	return value.print(os);
}

}

#endif

