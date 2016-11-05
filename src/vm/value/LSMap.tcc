#ifndef LS_MAP_TCC
#define LS_MAP_TCC

#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSArray.hpp"

#include <exception>

namespace ls {

template <typename K>
inline bool lsmap_less<K>::operator()(K lhs, K rhs) const {
	return lhs < rhs;
}

template <>
inline bool lsmap_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return *lhs < *rhs;
}

template <typename K, typename T>
LSValue* LSMap<K,T>::map_class(new LSClass("Map"));

template <typename K, typename T>
inline LSMap<K, T>::LSMap() {}

template <>
inline LSMap<LSValue*,LSValue*>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
	}
}
template <>
inline LSMap<LSValue*,int>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
}
template <>
inline LSMap<LSValue*,double>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
}
template <>
inline LSMap<int,LSValue*>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->second);
	}
}
template <>
inline LSMap<double,LSValue*>::~LSMap() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->second);
	}
}
template <typename K, typename T>
inline LSMap<K, T>::~LSMap() {}

/*
 * Map methods
 */
template <>
inline bool LSMap<LSValue*,LSValue*>::ls_insert(LSValue* key, LSValue* value) {
	auto it = lower_bound(key);
	if (it == end() || (*it->first != *key)) {
		emplace_hint(it, key->move_inc(), value->move_inc());
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,int>::ls_insert(LSValue* key, int value) {
	auto it = lower_bound(key);
	if (it == end() || (*it->first != *key)) {
		emplace_hint(it, key->move_inc(), value);
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,double>::ls_insert(LSValue* key, double value) {
	auto it = lower_bound(key);
	if (it == end() || (*it->first != *key)) {
		emplace_hint(it, key->move_inc(), value);
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(key);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,LSValue*>::ls_insert(int key, LSValue* value) {
	auto it = lower_bound(key);
	if (it == end() || it->first != key) {
		emplace_hint(it, key, value->move_inc());
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,int>::ls_insert(int key, int value) {
	bool r = emplace(key, value).second;
	if (refs == 0) delete this;
	return r;
}
template <>
inline bool LSMap<int,double>::ls_insert(int key, double value) {
	bool r = emplace(key, value).second;
	if (refs == 0) delete this;
	return r;
}


template <>
inline LSMap<LSValue*,LSValue*>* LSMap<LSValue*,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
	}
	clear();
	return this;
}
template <>
inline LSMap<LSValue*,int>* LSMap<LSValue*,int>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
	clear();
	return this;
}
template <>
inline LSMap<LSValue*,double>* LSMap<LSValue*,double>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->first);
	}
	clear();
	return this;
}
template <>
inline LSMap<int,LSValue*>* LSMap<int,LSValue*>::ls_clear() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(it->second);
	}
	clear();
	return this;
}
template <>
inline LSMap<int,int>* LSMap<int,int>::ls_clear() {
	clear();
	return this;
}
template <>
inline LSMap<int,double>* LSMap<int,double>::ls_clear() {
	clear();
	return this;
}

template <>
inline bool LSMap<LSValue*,LSValue*>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		LSValue::delete_ref(it->second);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,int>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<LSValue*,double>::ls_erase(LSValue* key) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_ref(it->first);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,LSValue*>::ls_erase(int key) {
	auto it = find(key);
	if (it != end()) {
		LSValue::delete_ref(it->second);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSMap<int,int>::ls_erase(int key) {
	bool r = erase(key);
	if (refs == 0) delete this;
	return r;
}
template <>
inline bool LSMap<int,double>::ls_erase(int key) {
	bool r = erase(key);
	if (refs == 0) delete this;
	return r;
}


template <>
inline LSValue* LSMap<LSValue*,LSValue*>::ls_look(LSValue* key, LSValue* def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		LSValue::delete_temporary(def);
		if (refs == 0) {
			LSValue* r = it->second->clone();
			delete this;
			return r;
		}
		return it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline int LSMap<LSValue*,int>::ls_look(LSValue* key, int def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline double LSMap<LSValue*,double>::ls_look(LSValue* key, double def) {
	auto it = find(key);
	LSValue::delete_temporary(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline LSValue* LSMap<int,LSValue*>::ls_look(int key, LSValue* def) {
	auto it = find(key);
	if (it != end()) {
		LSValue::delete_temporary(def);
		if (refs == 0) {
			LSValue* r = it->second->clone();
			delete this;
			return r;
		}
		return it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline int LSMap<int,int>::ls_look(int key, int def) {
	auto it = find(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}
template <>
inline double LSMap<int,double>::ls_look(int key, double def) {
	auto it = find(key);
	if (it != end()) {
		def = it->second;
	}
	if (refs == 0) delete this;
	return def;
}

template <typename K, typename V>
LSArray<V>* LSMap<K, V>::values() const {
	LSArray<V>* array = new LSArray<V>();
	for (auto i : *this) {
		array->push_clone(i.second);
	}
	LSValue::delete_temporary(this);
	return array;
}


/*
 * LSValue methods
 */

template <typename K, typename T>
inline bool LSMap<K,T>::isTrue() const {
	return not this->empty();
}

template <typename K, typename T>
inline bool LSMap<K, T>::in(K key) const {
	bool r = this->find(key) != this->end();
	LSValue::delete_temporary(this);
	return r;
}
template <>
inline bool LSMap<LSValue*, int>::in(LSValue* key) const {
	bool r = this->find(key) != this->end();
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(key);
	return r;
}
template <>
inline bool LSMap<LSValue*, double>::in(LSValue* key) const {
	bool r = this->find(key) != this->end();
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(key);
	return r;
}
template <>
inline bool LSMap<LSValue*, LSValue*>::in(LSValue* key) const {
	bool r = this->find(key) != this->end();
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(key);
	return r;
}

template <typename K, typename T>
template <typename K2, typename T2>
inline bool LSMap<K, T>::eq(const LSMap<K2, T2>* value) const {
	if (this->size() != value->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = value->begin();
	while (i != this->end()) {
		if (j == value->end()) return false;
		if (*i->first != *j->first) return false;
		if (*i->second != *j->second) return false;
		++j;
		++i;
	}
	return j == value->end();
}

template <typename K, typename T>
template <typename K2, typename T2>
inline bool LSMap<K, T>::lt(const LSMap<K2, T2>* map) const {
	auto i = this->begin();
	auto j = map->begin();

	for (; i != this->end(); ++i, ++j) {
		if (j == map->end()) return false;

		if (i->first < j->first) return true;
		if (j->first < i->first) return false;

		if (i->second < j->second) return true;
		if (j->second < i->second) return false;
	}
	return j != map->end();
}

template <typename K, typename V>
inline V LSMap<K, V>::at(const K key) const {
	try {
		auto map = (std::map<K, V, lsmap_less<K>>*) this;
		return map->at(key);
	} catch (std::exception&) {
		return V();
	}
}

template <typename K, typename T>
inline LSValue** LSMap<K, T>::atL(const LSValue*) {
	return &LSNull::null_var;
}
template <>
inline LSValue** LSMap<LSValue*, LSValue*>::atL(const LSValue* key) {
	try {
		return (LSValue**) &((std::map<LSValue*,LSValue*>*) this)->at((LSValue*) key);
	} catch (std::exception&) {
		return nullptr;
	}
}
template <>
inline LSValue** LSMap<int,LSValue*>::atL(const LSValue* key) {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue**) &((std::map<int,LSValue*>*) this)->at((int) n->value);
		} catch (std::exception&) {
			return nullptr;
		}
	}
	return nullptr;
}

template <typename K, typename T>
inline int* LSMap<K, T>::atLv(const LSValue*) const {
	return nullptr;
}
template <>
inline int* LSMap<LSValue*, int>::atLv(const LSValue* key) const {
	try {
		auto map = (std::map<LSValue*, int, lsmap_less<LSValue*>>*) this;
		return &map->at((LSValue*) key);
	} catch (std::exception&) {
		return nullptr;
	}
}

template <typename K, typename V>
inline std::ostream& LSMap<K, V>::print(std::ostream& os) const {
	os << "[";
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it != this->begin()) os << " ";
		os << it->first << " : " << it->second;
	}
	if (this->empty()) os << ':';
	return os << "]";
}

template <typename K, typename V>
inline std::string LSMap<K, V>::json() const {
	std::string res = "[";
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it != this->begin()) res += ",";
		res += ls::to_json(it->first);
		res += ":";
		res += ls::to_json(it->second);
	}
	return res + "]";
}

template <>
inline LSValue* LSMap<LSValue*, LSValue*>::clone() const {
	LSMap<LSValue*,LSValue*>* map = new LSMap<LSValue*,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second->clone_inc());
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*, int>::clone() const {
	LSMap<LSValue*,int>* map = new LSMap<LSValue*,int>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<LSValue*, double>::clone() const {
	LSMap<LSValue*,double>* map = new LSMap<LSValue*,double>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first->clone_inc(), it->second);
	}
	return map;
}
template <>
inline LSValue* LSMap<int, LSValue*>::clone() const {
	LSMap<int,LSValue*>* map = new LSMap<int,LSValue*>();
	for (auto it = begin(); it != end(); ++it) {
		map->emplace(it->first, it->second->clone_inc());
	}
	return map;
}
template <typename K, typename T>
inline LSValue* LSMap<K, T>::clone() const {
	return new LSMap<K, T>(*this);
}

template <typename K, typename T>
inline LSValue* LSMap<K, T>::getClass() const {
	return LSMap<K, T>::map_class;
}

template <typename K, typename T>
inline const BaseRawType* LSMap<K,T>::getRawType() const {
	return RawType::MAP;
}

}

#endif
