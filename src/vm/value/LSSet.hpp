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
	static LSValue* set_class;

	LSSet();
	LSSet(const LSSet<T>& other);
	virtual ~LSSet();

	/*
	 * LSSet methods;
	 */
	int ls_size();
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

	bool in(T) const;

	virtual LSValue* at(const LSValue* key) const override;
	virtual LSValue** atL(const LSValue* key) override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
	virtual int typeID() const override { return 7; }
	virtual const BaseRawType* getRawType() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSSet.tcc"
#endif

#endif
