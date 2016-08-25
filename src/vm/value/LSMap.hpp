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
	static LSValue* map_class;

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	bool ls_insert(K key, T value);
	LSMap<K,T>* ls_clear();
	bool ls_erase(K key);
	T ls_look(K key, T def);

	/*
	 * LSValue methods;
	 */
	virtual bool isTrue() const override;

	LSVALUE_OPERATORS

	virtual bool eq(const LSMap<LSValue*,LSValue*>*) const override;
	virtual bool eq(const LSMap<LSValue*,int>*) const override;
	virtual bool eq(const LSMap<LSValue*,double>*) const override;
	virtual bool eq(const LSMap<int,LSValue*>*) const override;
	virtual bool eq(const LSMap<int,int>*) const override;
	virtual bool eq(const LSMap<int,double>*) const override;

	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const override;
	virtual bool lt(const LSMap<LSValue*,int>*) const override;
	virtual bool lt(const LSMap<LSValue*,double>*) const override;
	virtual bool lt(const LSMap<int,LSValue*>*) const override;
	virtual bool lt(const LSMap<int,int>*) const override;
	virtual bool lt(const LSMap<int,double>*) const override;

	virtual LSValue* at(const LSValue* key) const override;
	virtual LSValue** atL(const LSValue* key) override;
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
