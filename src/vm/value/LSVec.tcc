#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "LSVec.hpp"
#include "LSVar.hpp"
#include <algorithm>

namespace ls {

template <typename T>
LSVec<T>::LSVec() {}

template <typename T>
LSVec<T>::LSVec(std::initializer_list<T> values_list) {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <typename T>
LSVec<T>::LSVec(const std::vector<T>& vec) : LSValue(), std::vector<T>(vec) {}

template <>
inline LSVec<LSValue*>::LSVec(const LSVec<LSValue*>& other) : LSValue(other), std::vector<LSValue*>() {
	reserve(other.size());
	for (LSValue* v : other) {
		push_back(LSValue::clone_inc(v));
	}
}
template <typename T>
inline LSVec<T>::LSVec(const LSVec<T>& other) : LSValue(other), std::vector<T>(other) {
}

template <typename T>
LSVec<T>::LSVec(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone((T) LSValue::parse(it.value()));
	}
}

/*
template <>
inline LSVec<int>::LSVec(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone(((LSNumber*) LSValue::parse(it.value()))->value);
	}
}
*/

template <>
inline LSVec<LSValue*>::~LSVec() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
}
template <typename T>
LSVec<T>::~LSVec() {}

template <>
inline LSValue* LSVec<LSValue*>::ls_push(LSVec<LSValue*>* vec, LSValue* value)
{
	if (vec) vec->push_back(LSValue::move_inc(value));
	return vec;
}
template <typename T>
inline LSValue* LSVec<T>::ls_push(LSVec<T>* vec, T value)
{
	if (vec) vec->push_back(value);
	return vec;
}






template <typename T>
bool LSVec<T>::isTrue() const {
	return this->size() > 0;
}

template <>
inline bool LSVec<LSValue*>::eq(const LSVec<LSValue*>* that) const
{
	if (this->size() != that->size()) return false;
	auto i = this->begin();
	auto j = that->begin();

	while (i != this->end()) {
		if (*i == nullptr && *j == nullptr) continue;
		if (*i == nullptr) return false;
		if (*j == nullptr) return false;
		if (**i != **j) return false;
	}
	return true;
}

template <>
inline bool LSVec<void*>::eq(const LSVec<LSValue*>*) const
{
	return false;
}

template <typename T>
bool LSVec<T>::eq(const LSVec<LSValue*>* that) const
{
	if (this->size() != that->size()) return false;
	auto i = this->begin();
	auto j = that->begin();

	while (i != this->end()) {
		if (*j == nullptr) return false;
		LSVar* var = dynamic_cast<LSVar*>(*j);
		if (var == nullptr) return false;
		if (*i != var->real) return false;
	}
	return true;
}

template <>
inline bool LSVec<void*>::eq(const LSVec<void*>* that) const
{
	return *this == *that;
}

template <typename T>
inline bool LSVec<T>::eq(const LSVec<void*>*) const
{
	return false;
}

template <>
inline bool LSVec<LSValue*>::eq(const LSVec<int32_t>* that) const
{
	return that->eq(this);
}

template <>
inline bool LSVec<void*>::eq(const LSVec<int32_t>*) const
{
	return false;
}

template <>
inline bool LSVec<int32_t>::eq(const LSVec<int32_t>* that) const
{
	return *this == *that;
}

template <>
inline bool LSVec<double>::eq(const LSVec<int32_t>* that) const
{
	if (this->size() != that->size()) return false;
	auto i = this->begin();
	auto j = that->begin();

	while (i != this->end()) {
		if (*i != *j) return false;
	}
	return true;
}

template <>
inline bool LSVec<LSValue*>::eq(const LSVec<double>* that) const
{
	return that->eq(this);
}

template <>
inline bool LSVec<void*>::eq(const LSVec<double>*) const
{
	return false;
}

template <>
inline bool LSVec<double>::eq(const LSVec<double>* that) const
{
	return *this == *that;
}

template <>
inline bool LSVec<int32_t>::eq(const LSVec<double>* that) const
{
	if (this->size() != that->size()) return false;
	auto i = this->begin();
	auto j = that->begin();

	while (i != this->end()) {
		if (*i != *j) return false;
	}
	return true;
}


template <typename T>
LSValue* LSVec<T>::clone() const {
	return new LSVec<T>(*this);
}

template <typename T>
std::ostream& LSVec<T>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSVec<void*>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << "<function>";
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSVec<LSValue*>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << *i;
	}
	os << "]";
	return os;
}

template <typename T>
std::string LSVec<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::ostringstream oss;
		oss << *i;
		res += oss.str();
	}
	return res + "]";
}

template <>
inline std::string LSVec<LSValue*>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = (*i)->to_json();
		res += json;
	}
	return res + "]";
}



} // end of namespace ls

#endif

