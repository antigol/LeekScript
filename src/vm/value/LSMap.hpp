#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K = LSValue*, typename T = LSValue*>
class LSMap : public LSValue, public std::map<K, T, lsmap_less<K>> {
public:
	static LSValue* map_class;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	bool ls_insert(K key, T value);
	LSMap<K, T>* ls_clear();
	bool ls_erase(K key);
	T ls_look(K key, T def);
	LSArray<T>* values() const;

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	LSVALUE_OPERATORS

	template <typename K2, typename T2>
	bool eq(const LSMap<K2, T2>*) const;

	template <typename K2, typename T2>
	bool lt(const LSMap<K2, T2>*) const;

	T at(const K key) const;

	virtual LSValue** atL(const LSValue* key) override;
	int* atLv(const LSValue* key) const;

	bool in(K) const;

	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
	virtual int typeID() const override { return 6; }
	virtual const BaseRawType* getRawType() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
