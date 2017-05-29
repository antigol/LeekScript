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
	static LSValue* clazz;

	LSSet();
	LSSet(const LSSet<T>& other);
	virtual ~LSSet();

	/*
	 * LSSet methods
	 */
	int ls_size();
	bool ls_insert(T value);
	LSSet<T>* ls_clear();
	bool ls_erase(T value);
	bool ls_contains(T value);
	template <class T2>
	bool set_lt(const LSSet<T2>* set) const;

	/*
	 * LSValue methods
	 */
 	virtual bool to_bool() const override;
	virtual bool ls_not() const override;
	virtual LSValue* add_eq(LSValue* v) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	bool in(const T) const;
	int abso() const override;
	virtual std::ostream& dump(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSSet.tcc"
#endif

#endif
