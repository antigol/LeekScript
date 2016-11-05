#ifndef LS_SET_TCC
#define LS_SET_TCC

#include "LSSet.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

namespace ls {

template <>
inline bool lsset_less<LSValue*>::operator()(LSValue* lhs, LSValue* rhs) const {
	return *lhs < *rhs;
}

template <typename T>
inline bool lsset_less<T>::operator()(T lhs, T rhs) const {
	return lhs < rhs;
}

template <typename T>
LSValue* LSSet<T>::set_class(new LSClass("Set"));

template <>
inline LSSet<LSValue*>::LSSet() {}
template <>
inline LSSet<double>::LSSet() {}
template <>
inline LSSet<int>::LSSet() {}

template <>
inline LSSet<LSValue*>::LSSet(const LSSet<LSValue*>& other) : LSValue(other), std::set<LSValue*, lsset_less<LSValue*>>() {
	for (LSValue* v : other) {
		insert(end(), v->clone_inc());
	}
}
template <typename T>
inline LSSet<T>::LSSet(const LSSet<T>& other) : LSValue(other), std::set<T, lsset_less<T>>(other) {
}


template <>
inline LSSet<LSValue*>::~LSSet() {
	for (auto it = begin(); it != end(); ++it) {
		LSValue::delete_ref(*it);
	}
}
template <typename T>
inline LSSet<T>::~LSSet() {
}

template <typename T>
int LSSet<T>::ls_size() {
	int s = this->size();
	LSValue::delete_temporary(this);
	return s;
}

template <>
inline bool LSSet<LSValue*>::ls_insert(LSValue* value) {
	auto it = lower_bound(value);
	if (it == end() || (**it != *value)) {
		insert(it, value->move_inc());
		if (refs == 0) delete this;
		return true;
	}
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return false;
}
template <typename T>
inline bool LSSet<T>::ls_insert(T value) {
	bool r = this->insert(value).second;
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSSet<LSValue*> *LSSet<LSValue*>::ls_clear() {
	for (LSValue* v : *this) LSValue::delete_ref(v);
	this->clear();
	return this;
}
template <typename T>
inline LSSet<T> *LSSet<T>::ls_clear() {
	this->clear();
	return this;
}

template <>
inline bool LSSet<LSValue*>::ls_erase(LSValue* value) {
	auto it = find(value);
	LSValue::delete_temporary(value);
	if (it == end()) {
		if (refs == 0) delete this;
		return false;
	} else {
		LSValue::delete_ref(*it);
		erase(it);
		if (refs == 0) delete this;
		return true;
	}
}
template <typename T>
inline bool LSSet<T>::ls_erase(T value) {
	bool r = this->erase(value);
	if (refs == 0) delete this;
	return r;
}

template <>
inline bool LSSet<LSValue*>::ls_contains(LSValue* value) {
	bool r = count(value);
	LSValue::delete_temporary(value);
	if (refs == 0) delete this;
	return r;
}
template <typename T>
inline bool LSSet<T>::ls_contains(T value) {
	bool r = this->count(value);
	if (refs == 0) delete this;
	return r;
}

template <typename T>
bool LSSet<T>::isTrue() const {
	return !this->empty();
}

template <>
inline bool LSSet<LSValue*>::eq(const LSSet<LSValue*>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		if (**it1 != **it2) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::eq(const LSSet<LSValue*>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		LSNumber* v2 = dynamic_cast<LSNumber*>(*it2);
		if (!v2) return false;
		if (*it1 != v2->value) return false;
		++it1;
		++it2;
	}
	return true;
}

template <>
inline bool LSSet<LSValue*>::eq(const LSSet<int>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		LSNumber* v1 = dynamic_cast<LSNumber*>(*it1);
		if (!v1) return false;
		if (*it2 != v1->value) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::eq(const LSSet<int>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		if (*it1 != *it2) return false;
		++it1;
		++it2;
	}
	return true;
}

template <>
inline bool LSSet<LSValue*>::eq(const LSSet<double>* other) const {
	if (size() != other->size()) return false;
	auto it1 = begin();
	auto it2 = other->begin();
	while (it1 != end()) {
		LSNumber* v1 = dynamic_cast<LSNumber*>(*it1);
		if (!v1) return false;
		if (*it2 != v1->value) return false;
		++it1;
		++it2;
	}
	return true;
}
template <typename T>
inline bool LSSet<T>::eq(const LSSet<double>* other) const {
	if (this->size() != other->size()) return false;
	auto it1 = this->begin();
	auto it2 = other->begin();
	while (it1 != this->end()) {
		if (*it1 != *it2) return false;
		++it1;
		++it2;
	}
	return true;
}

template <>
inline bool LSSet<LSValue*>::lt(const LSSet<LSValue*>* set) const {
	return std::lexicographical_compare(begin(), end(), set->begin(), set->end(), [](LSValue* a, LSValue* b) -> bool {
		return *a < *b;
	});
}
template <typename T>
inline bool LSSet<T>::lt(const LSSet<LSValue*>* set) const {
	auto i = this->begin();
	auto j = set->begin();
	while (i != this->end()) {
		if (j == set->end()) return false;
		if ((*j)->typeID() < 3) return false;
		if (3 < (*j)->typeID()) return true;
		if (*i < ((LSNumber*) *j)->value) return true;
		if (((LSNumber*) *j)->value < *i) return false;
		++i; ++j;
	}
	return (j != set->end());
}

template <>
inline bool LSSet<LSValue*>::lt(const LSSet<int>* v) const {
	auto i = begin();
	auto j = v->begin();
	while (i != end()) {
		if (j == v->end()) return false;
		if (3 < (*i)->typeID()) return false;
		if ((*i)->typeID() < 3) return true;
		if (((LSNumber*) *i)->value < *j) return true;
		if (*j < ((LSNumber*) *i)->value) return false;
		++i; ++j;
	}
	return (j != v->end());
}
template <typename T>
inline bool LSSet<T>::lt(const LSSet<int>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}

template <>
inline bool LSSet<LSValue*>::lt(const LSSet<double>* v) const {
	auto i = begin();
	auto j = v->begin();
	while (i != end()) {
		if (j == v->end()) return false;
		if (3 < (*i)->typeID()) return false;
		if ((*i)->typeID() < 3) return true;
		if (((LSNumber*) *i)->value < *j) return true;
		if (*j < ((LSNumber*) *i)->value) return false;
		++i; ++j;
	}
	return (j != v->end());
}
template <typename T>
inline bool LSSet<T>::lt(const LSSet<double>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}

template <>
inline bool LSSet<LSValue*>::in(LSValue* value) const {
	bool r = count(value);
	LSValue::delete_temporary(this);
	LSValue::delete_temporary(value);
	return r;
}
template <typename T>
inline bool LSSet<T>::in(T value) const {
	bool r = this->count(value);
	LSValue::delete_temporary(this);
	return r;
}

template <typename T>
LSValue* LSSet<T>::at(const LSValue* ) const {
	return LSNull::get();
}

template <typename T>
LSValue** LSSet<T>::atL(const LSValue* ) {
	return nullptr;
}

template <typename T>
inline std::ostream& LSSet<T>::print(std::ostream& os) const {
	os << "<";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << *i;
	}
	os << ">";
	return os;
}

template <typename T>
inline std::string LSSet<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = ls::to_json(*i);
		res += json;
	}
	return res + "]";
}

template <typename T>
inline LSValue* LSSet<T>::clone() const {
	return new LSSet<T>(*this);
}

template <typename T>
LSValue*LSSet<T>::getClass() const {
	return LSSet<T>::set_class;
}

template <typename T>
const BaseRawType*LSSet<T>::getRawType() const {
	return RawType::SET;
}


}

#endif
