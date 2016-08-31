#ifndef LS_SET_BASE
#define LS_SET_BASE

#include "../LSValue.hpp"
#include <set>

namespace ls {

template <typename K>
struct lsset_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename T>
class LSSet : public LSValue, public std::set<T, lsset_less<T>> {
public:
	LSSet();
	LSSet(const LSSet<T>& other);
	virtual ~LSSet();

	/*
	 * LSSet methods;
	 */
	bool ls_insert(T value);
	LSSet<T>* ls_clear();
	bool ls_erase(T value);
	bool ls_contains(T value);

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	LSVALUE_OPERATORS

	virtual bool eq(const LSSet<LSValue*>*) const override;
	virtual bool eq(const LSSet<int>*) const override;
	virtual bool eq(const LSSet<double>*) const override;
	virtual bool lt(const LSSet<LSValue*>*) const override;
	virtual bool lt(const LSSet<int>*) const;
	virtual bool lt(const LSSet<double>*) const;

	bool in(LSValue*) const;
	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual int typeID() const override { return 7; }
	virtual RawType getRawType() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSSet.tcc"
#endif

#endif
