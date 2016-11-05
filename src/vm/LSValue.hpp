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
class LSNull;
class LSBoolean;
class LSNumber;
class LSString;
template <typename T> class LSArray;
template <typename K, typename T> class LSMap;
template <typename T> class LSSet;
class LSFunction;
class LSObject;
class LSClass;
class Context;

#define LSVALUE_OPERATORS \
	LSValue* ls_radd(LSValue* value) override     { return value->ls_add(this); } \
	LSValue* ls_radd_eq(LSValue* value) override  { return value->ls_add_eq(this); } \
	LSValue* ls_rsub(LSValue* value) override     { return value->ls_sub(this); } \
	LSValue* ls_rsub_eq(LSValue* value) override  { return value->ls_sub_eq(this); } \
	LSValue* ls_rmul(LSValue* value) override     { return value->ls_mul(this); } \
	LSValue* ls_rmul_eq(LSValue* value) override  { return value->ls_mul_eq(this); } \
	LSValue* ls_rdiv(LSValue* value) override     { return value->ls_div(this); } \
	LSValue* ls_rint_div(LSValue* value) override { return value->ls_div(this); } \
	LSValue* ls_rdiv_eq(LSValue* value) override  { return value->ls_div_eq(this); } \
	LSValue* ls_rpow(LSValue* value) override     { return value->ls_pow(this); } \
	LSValue* ls_rpow_eq(LSValue* value) override  { return value->ls_pow_eq(this); } \
	LSValue* ls_rmod(LSValue* value) override     { return value->ls_mod(this); } \
	LSValue* ls_rmod_eq(LSValue* value) override  { return value->ls_mod_eq(this); } \
	bool req(const LSValue* value) const override { return value->eq(this); } \
	bool rlt(const LSValue* value) const override { return value->lt(this); } \

class LSValue {
public:

	static int obj_count;
	static int obj_deleted;
	#if DEBUG_LEAKS_DETAILS
		static std::map<void*, LSValue*> & objs() {
			static std::map<void*, LSValue*> objs;
			return objs;
		}
	#endif

	int refs = 0;
	bool native = false;

	LSValue();
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual LSValue* ls_minus();
	virtual LSValue* ls_not();
	virtual LSValue* ls_tilde();
	virtual LSValue* ls_preinc(); // ++x
	virtual LSValue* ls_inc(); // x++
	virtual LSValue* ls_predec();
	virtual LSValue* ls_dec();

	LSValue* ls_add(LSValue* value) {
		return value->ls_radd(this);
	}
	virtual LSValue* ls_radd(LSValue*) = 0;
	virtual LSValue* ls_add(LSNull*);
	virtual LSValue* ls_add(LSBoolean*);
	virtual LSValue* ls_add(LSNumber*);
	virtual LSValue* ls_add(LSString*);
	virtual LSValue* ls_add(LSArray<LSValue*>*);
	virtual LSValue* ls_add(LSArray<int>*);
	virtual LSValue* ls_add(LSArray<double>*);
	virtual LSValue* ls_add(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_add(LSMap<LSValue*,int>*);
	virtual LSValue* ls_add(LSMap<LSValue*,double>*);
	virtual LSValue* ls_add(LSMap<int,LSValue*>*);
	virtual LSValue* ls_add(LSMap<int,int>*);
	virtual LSValue* ls_add(LSMap<int,double>*);
	virtual LSValue* ls_add(LSSet<LSValue*>*);
	virtual LSValue* ls_add(LSSet<int>*);
	virtual LSValue* ls_add(LSSet<double>*);
	virtual LSValue* ls_add(LSObject*);
	virtual LSValue* ls_add(LSFunction*);
	virtual LSValue* ls_add(LSClass*);

	LSValue* ls_add_eq(LSValue* value) {
		return value->ls_radd_eq(this);
	}
	virtual LSValue* ls_radd_eq(LSValue*) = 0;
	virtual LSValue* ls_add_eq(LSNull*);
	virtual LSValue* ls_add_eq(LSBoolean*);
	virtual LSValue* ls_add_eq(LSNumber*);
	virtual LSValue* ls_add_eq(LSString*);
	virtual LSValue* ls_add_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_add_eq(LSArray<int>*);
	virtual LSValue* ls_add_eq(LSArray<double>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_add_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_add_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_add_eq(LSMap<int,int>*);
	virtual LSValue* ls_add_eq(LSMap<int,double>*);
	virtual LSValue* ls_add_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_add_eq(LSSet<int>*);
	virtual LSValue* ls_add_eq(LSSet<double>*);
	virtual LSValue* ls_add_eq(LSObject*);
	virtual LSValue* ls_add_eq(LSFunction*);
	virtual LSValue* ls_add_eq(LSClass*);

	LSValue* ls_sub(LSValue* value) {
		return value->ls_rsub(this);
	}
	virtual LSValue* ls_rsub(LSValue*) = 0;
	virtual LSValue* ls_sub(LSNull*);
	virtual LSValue* ls_sub(LSBoolean*);
	virtual LSValue* ls_sub(LSNumber*);
	virtual LSValue* ls_sub(LSString*);
	virtual LSValue* ls_sub(LSArray<LSValue*>*);
	virtual LSValue* ls_sub(LSArray<int>*);
	virtual LSValue* ls_sub(LSArray<double>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,int>*);
	virtual LSValue* ls_sub(LSMap<LSValue*,double>*);
	virtual LSValue* ls_sub(LSMap<int,LSValue*>*);
	virtual LSValue* ls_sub(LSMap<int,int>*);
	virtual LSValue* ls_sub(LSMap<int,double>*);
	virtual LSValue* ls_sub(LSSet<LSValue*>*);
	virtual LSValue* ls_sub(LSSet<int>*);
	virtual LSValue* ls_sub(LSSet<double>*);
	virtual LSValue* ls_sub(LSObject*);
	virtual LSValue* ls_sub(LSFunction*);
	virtual LSValue* ls_sub(LSClass*);

	LSValue* ls_sub_eq(LSValue* value) {
		return value->ls_rsub_eq(this);
	}
	virtual LSValue* ls_rsub_eq(LSValue*) = 0;
	virtual LSValue* ls_sub_eq(LSNull*);
	virtual LSValue* ls_sub_eq(LSBoolean*);
	virtual LSValue* ls_sub_eq(LSNumber*);
	virtual LSValue* ls_sub_eq(LSString*);
	virtual LSValue* ls_sub_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_sub_eq(LSArray<int>*);
	virtual LSValue* ls_sub_eq(LSArray<double>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_sub_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_sub_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_sub_eq(LSMap<int,int>*);
	virtual LSValue* ls_sub_eq(LSMap<int,double>*);
	virtual LSValue* ls_sub_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_sub_eq(LSSet<int>*);
	virtual LSValue* ls_sub_eq(LSSet<double>*);
	virtual LSValue* ls_sub_eq(LSObject*);
	virtual LSValue* ls_sub_eq(LSFunction*);
	virtual LSValue* ls_sub_eq(LSClass*);

	LSValue* ls_mul(LSValue* value) {
		return value->ls_rmul(this);
	}
	virtual LSValue* ls_rmul(LSValue*) = 0;
	virtual LSValue* ls_mul(LSNull*);
	virtual LSValue* ls_mul(LSBoolean*);
	virtual LSValue* ls_mul(LSNumber*);
	virtual LSValue* ls_mul(LSString*);
	virtual LSValue* ls_mul(LSArray<LSValue*>*);
	virtual LSValue* ls_mul(LSArray<int>*);
	virtual LSValue* ls_mul(LSArray<double>*);
	virtual LSValue* ls_mul(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mul(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mul(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mul(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mul(LSMap<int,int>*);
	virtual LSValue* ls_mul(LSMap<int,double>*);
	virtual LSValue* ls_mul(LSSet<LSValue*>*);
	virtual LSValue* ls_mul(LSSet<int>*);
	virtual LSValue* ls_mul(LSSet<double>*);
	virtual LSValue* ls_mul(LSObject*);
	virtual LSValue* ls_mul(LSFunction*);
	virtual LSValue* ls_mul(LSClass*);

	LSValue* ls_mul_eq(LSValue* value) {
		return value->ls_rmul_eq(this);
	}
	virtual LSValue* ls_rmul_eq(LSValue*) = 0;
	virtual LSValue* ls_mul_eq(LSNull*);
	virtual LSValue* ls_mul_eq(LSBoolean*);
	virtual LSValue* ls_mul_eq(LSNumber*);
	virtual LSValue* ls_mul_eq(LSString*);
	virtual LSValue* ls_mul_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_mul_eq(LSArray<int>*);
	virtual LSValue* ls_mul_eq(LSArray<double>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mul_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mul_eq(LSMap<int,int>*);
	virtual LSValue* ls_mul_eq(LSMap<int,double>*);
	virtual LSValue* ls_mul_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_mul_eq(LSSet<int>*);
	virtual LSValue* ls_mul_eq(LSSet<double>*);
	virtual LSValue* ls_mul_eq(LSObject*);
	virtual LSValue* ls_mul_eq(LSFunction*);
	virtual LSValue* ls_mul_eq(LSClass*);

	LSValue* ls_div(LSValue* value) {
		return value->ls_rdiv(this);
	}
	virtual LSValue* ls_rdiv(LSValue*) = 0;
	virtual LSValue* ls_div(LSNull*);
	virtual LSValue* ls_div(LSBoolean*);
	virtual LSValue* ls_div(LSNumber*);
	virtual LSValue* ls_div(LSString*);
	virtual LSValue* ls_div(LSArray<LSValue*>*);
	virtual LSValue* ls_div(LSArray<int>*);
	virtual LSValue* ls_div(LSArray<double>*);
	virtual LSValue* ls_div(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_div(LSMap<LSValue*,int>*);
	virtual LSValue* ls_div(LSMap<LSValue*,double>*);
	virtual LSValue* ls_div(LSMap<int,LSValue*>*);
	virtual LSValue* ls_div(LSMap<int,int>*);
	virtual LSValue* ls_div(LSMap<int,double>*);
	virtual LSValue* ls_div(LSSet<LSValue*>*);
	virtual LSValue* ls_div(LSSet<int>*);
	virtual LSValue* ls_div(LSSet<double>*);
	virtual LSValue* ls_div(LSObject*);
	virtual LSValue* ls_div(LSFunction*);
	virtual LSValue* ls_div(LSClass*);

	LSValue* ls_int_div(LSValue* value) {
		return value->ls_rint_div(this);
	}
	virtual LSValue* ls_rint_div(LSValue*) = 0;
	virtual LSValue* ls_int_div(LSNull*);
	virtual LSValue* ls_int_div(LSBoolean*);
	virtual LSValue* ls_int_div(LSNumber*);
	virtual LSValue* ls_int_div(LSString*);
	virtual LSValue* ls_int_div(LSArray<LSValue*>*);
	virtual LSValue* ls_int_div(LSArray<int>*);
	virtual LSValue* ls_int_div(LSArray<double>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,int>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,double>*);
	virtual LSValue* ls_int_div(LSMap<int,LSValue*>*);
	virtual LSValue* ls_int_div(LSMap<int,int>*);
	virtual LSValue* ls_int_div(LSMap<int,double>*);
	virtual LSValue* ls_int_div(LSSet<LSValue*>*);
	virtual LSValue* ls_int_div(LSSet<int>*);
	virtual LSValue* ls_int_div(LSSet<double>*);
	virtual LSValue* ls_int_div(LSObject*);
	virtual LSValue* ls_int_div(LSFunction*);
	virtual LSValue* ls_int_div(LSClass*);

	LSValue* ls_div_eq(LSValue* value) {
		return value->ls_rdiv_eq(this);
	}
	virtual LSValue* ls_rdiv_eq(LSValue*) = 0;
	virtual LSValue* ls_div_eq(LSNull*);
	virtual LSValue* ls_div_eq(LSBoolean*);
	virtual LSValue* ls_div_eq(LSNumber*);
	virtual LSValue* ls_div_eq(LSString*);
	virtual LSValue* ls_div_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_div_eq(LSArray<int>*);
	virtual LSValue* ls_div_eq(LSArray<double>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_div_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_div_eq(LSMap<int,int>*);
	virtual LSValue* ls_div_eq(LSMap<int,double>*);
	virtual LSValue* ls_div_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_div_eq(LSSet<int>*);
	virtual LSValue* ls_div_eq(LSSet<double>*);
	virtual LSValue* ls_div_eq(LSObject*);
	virtual LSValue* ls_div_eq(LSFunction*);
	virtual LSValue* ls_div_eq(LSClass*);

	LSValue* ls_pow(LSValue* value) {
		return value->ls_rpow(this);
	}
	virtual LSValue* ls_rpow(LSValue*) = 0;
	virtual LSValue* ls_pow(LSNull*);
	virtual LSValue* ls_pow(LSBoolean*);
	virtual LSValue* ls_pow(LSNumber*);
	virtual LSValue* ls_pow(LSString*);
	virtual LSValue* ls_pow(LSArray<LSValue*>*);
	virtual LSValue* ls_pow(LSArray<int>*);
	virtual LSValue* ls_pow(LSArray<double>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,int>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,double>*);
	virtual LSValue* ls_pow(LSMap<int,LSValue*>*);
	virtual LSValue* ls_pow(LSMap<int,int>*);
	virtual LSValue* ls_pow(LSMap<int,double>*);
	virtual LSValue* ls_pow(LSSet<LSValue*>*);
	virtual LSValue* ls_pow(LSSet<int>*);
	virtual LSValue* ls_pow(LSSet<double>*);
	virtual LSValue* ls_pow(LSObject*);
	virtual LSValue* ls_pow(LSFunction*);
	virtual LSValue* ls_pow(LSClass*);

	LSValue* ls_pow_eq(LSValue* value) {
		return value->ls_rpow_eq(this);
	}
	virtual LSValue* ls_rpow_eq(LSValue*) = 0;
	virtual LSValue* ls_pow_eq(LSNull*);
	virtual LSValue* ls_pow_eq(LSBoolean*);
	virtual LSValue* ls_pow_eq(LSNumber*);
	virtual LSValue* ls_pow_eq(LSString*);
	virtual LSValue* ls_pow_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_pow_eq(LSArray<int>*);
	virtual LSValue* ls_pow_eq(LSArray<double>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_pow_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_pow_eq(LSMap<int,int>*);
	virtual LSValue* ls_pow_eq(LSMap<int,double>*);
	virtual LSValue* ls_pow_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_pow_eq(LSSet<int>*);
	virtual LSValue* ls_pow_eq(LSSet<double>*);
	virtual LSValue* ls_pow_eq(LSObject*);
	virtual LSValue* ls_pow_eq(LSFunction*);
	virtual LSValue* ls_pow_eq(LSClass*);

	LSValue* ls_mod(LSValue* value) {
		return value->ls_rmod(this);
	}
	virtual LSValue* ls_rmod(LSValue*) = 0;
	virtual LSValue* ls_mod(LSNull*);
	virtual LSValue* ls_mod(LSBoolean*);
	virtual LSValue* ls_mod(LSNumber*);
	virtual LSValue* ls_mod(LSString*);
	virtual LSValue* ls_mod(LSArray<LSValue*>*);
	virtual LSValue* ls_mod(LSArray<int>*);
	virtual LSValue* ls_mod(LSArray<double>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mod(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mod(LSMap<int,int>*);
	virtual LSValue* ls_mod(LSMap<int,double>*);
	virtual LSValue* ls_mod(LSSet<LSValue*>*);
	virtual LSValue* ls_mod(LSSet<int>*);
	virtual LSValue* ls_mod(LSSet<double>*);
	virtual LSValue* ls_mod(LSObject*);
	virtual LSValue* ls_mod(LSFunction*);
	virtual LSValue* ls_mod(LSClass*);

	LSValue* ls_mod_eq(LSValue* value) {
		return value->ls_rmod_eq(this);
	}
	virtual LSValue* ls_rmod_eq(LSValue*) = 0;
	virtual LSValue* ls_mod_eq(LSNull*);
	virtual LSValue* ls_mod_eq(LSBoolean*);
	virtual LSValue* ls_mod_eq(LSNumber*);
	virtual LSValue* ls_mod_eq(LSString*);
	virtual LSValue* ls_mod_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_mod_eq(LSArray<int>*);
	virtual LSValue* ls_mod_eq(LSArray<double>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mod_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mod_eq(LSMap<int,int>*);
	virtual LSValue* ls_mod_eq(LSMap<int,double>*);
	virtual LSValue* ls_mod_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_mod_eq(LSSet<int>*);
	virtual LSValue* ls_mod_eq(LSSet<double>*);
	virtual LSValue* ls_mod_eq(LSObject*);
	virtual LSValue* ls_mod_eq(LSFunction*);
	virtual LSValue* ls_mod_eq(LSClass*);

	bool operator == (const LSValue& value) const {
		return value.req(this);
	}
	bool operator != (const LSValue& value) const {
		return !value.req(this);
	}
	virtual bool req(const LSValue*) const = 0;
	virtual bool eq(const LSNull*) const;
	virtual bool eq(const LSBoolean*) const;
	virtual bool eq(const LSNumber*) const;
	virtual bool eq(const LSString*) const;
	virtual bool eq(const LSArray<LSValue*>*) const;
	virtual bool eq(const LSArray<int>*) const;
	virtual bool eq(const LSArray<double>*) const;
	virtual bool eq(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool eq(const LSMap<LSValue*,int>*) const;
	virtual bool eq(const LSMap<LSValue*,double>*) const;
	virtual bool eq(const LSMap<int,LSValue*>*) const;
	virtual bool eq(const LSMap<int,int>*) const;
	virtual bool eq(const LSMap<int,double>*) const;
	virtual bool eq(const LSMap<double,LSValue*>*) const;
	virtual bool eq(const LSMap<double,int>*) const;
	virtual bool eq(const LSMap<double,double>*) const;
	virtual bool eq(const LSSet<LSValue*>*) const;
	virtual bool eq(const LSSet<int>*) const;
	virtual bool eq(const LSSet<double>*) const;
	virtual bool eq(const LSFunction*) const;
	virtual bool eq(const LSObject*) const;
	virtual bool eq(const LSClass*) const;

	bool operator < (const LSValue& value) const {
		return value.rlt(this);
	}
	bool operator > (const LSValue& value) const {
		return !(*this == value) && !(*this < value);
	}
	bool operator <=(const LSValue& value) const {
		return (*this == value) || (*this < value);
	}
	bool operator >=(const LSValue& value) const {
		return !(*this < value);
	}
	virtual bool rlt(const LSValue*) const = 0;
	virtual bool lt(const LSNull*) const;
	virtual bool lt(const LSBoolean*) const;
	virtual bool lt(const LSNumber*) const;
	virtual bool lt(const LSString*) const;
	virtual bool lt(const LSArray<LSValue*>*) const;
	virtual bool lt(const LSArray<int>*) const;
	virtual bool lt(const LSArray<double>*) const;
	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool lt(const LSMap<LSValue*,int>*) const;
	virtual bool lt(const LSMap<LSValue*,double>*) const;
	virtual bool lt(const LSMap<int,LSValue*>*) const;
	virtual bool lt(const LSMap<int,int>*) const;
	virtual bool lt(const LSMap<int,double>*) const;
	virtual bool lt(const LSMap<double,LSValue*>*) const;
	virtual bool lt(const LSMap<double,int>*) const;
	virtual bool lt(const LSMap<double,double>*) const;
	virtual bool lt(const LSSet<LSValue*>*) const;
	virtual bool lt(const LSSet<int>*) const;
	virtual bool lt(const LSSet<double>*) const;
	virtual bool lt(const LSFunction*) const;
	virtual bool lt(const LSObject*) const;
	virtual bool lt(const LSClass*) const;

	virtual bool in(LSValue*) const { return false; }

	virtual LSValue* at(const LSValue* key) const;
	virtual LSValue** atL(const LSValue* key);

	virtual LSValue* attr(const LSValue* key) const;
	virtual LSValue** attrL(const LSValue* key);

	virtual LSValue* range(int start, int end) const;
	virtual LSValue* rangeL(int start, int end);

	virtual LSValue* abso() const;

	virtual std::ostream& print(std::ostream&) const = 0;
	virtual std::string json() const = 0;
	std::string to_json() const;

	virtual LSValue* clone() const = 0;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual LSValue* getClass() const = 0;

	bool isInteger() const;

	virtual int typeID() const = 0;

	virtual const BaseRawType* getRawType() const = 0;

	static LSValue* parse(Json& json);

	static void delete_ref(LSValue* value);
	static void delete_temporary(const LSValue* const value);
};

inline LSValue* LSValue::clone_inc() {
	if (native) {
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs++;
		return copy;
	}
}

inline LSValue* LSValue::move() {
	if (native) {
		return this;
	} else {
		if (refs == 0) {
			return this;
		}
		return clone();
	}
}

inline LSValue* LSValue::move_inc() {
	if (native) {
		return this;
	} else if (refs == 0) {
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
	if (value->native) return;
	if (value->refs == 0) return;

	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(const LSValue* const value) {

	if (value == nullptr) return;
	if (value->native) return;

	if (value->refs == 0) {
		delete value;
	}
}

inline std::ostream& operator << (std::ostream& os, const LSValue& value) {
	return value.print(os);
}

}

namespace ls {
	template <typename T>
	std::string to_json(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string to_json(LSValue* v) {
		return v->to_json();
	}
	template <typename T>
	bool equals(const T v1, const T v2);
	template <>
	inline bool equals(ls::LSValue* v1, ls::LSValue* v2) {
		return v1->operator == (*v2);
	}
	template <typename T>
	bool equals(const T v1, const T v2) {
		return v1 == v2;
	}
}

namespace std {
	std::string to_string(ls::LSValue* value);
	std::ostream& operator << (std::ostream&, const ls::LSValue*);
}

#endif
