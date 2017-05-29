#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K, typename T>
class LSMap : public LSValue, public std::map<K, T, lsmap_less<K>> {
public:
	static LSValue* clazz;

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
	template <class K2, class T2>
	bool map_equals(const LSMap<K2, T2>* map) const;
	template <class K2, class T2>
	bool map_lt(const LSMap<K2, T2>* map) const;

	/*
	 * LSValue methods;
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;

	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	T at(const K key) const;

	virtual LSValue** atL(const LSValue* key) override;
	T* atL_base(K key) const;

	bool in(K) const;

	virtual std::ostream& dump(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
