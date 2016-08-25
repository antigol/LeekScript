#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "../LSValue.hpp"
#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSBoolean.hpp"
#include "LSFunction.hpp"
#include "LSObject.hpp"
#include "LSSet.hpp"

#include <algorithm>

namespace ls {

template <class T>
LSValue* LSArray<T>::array_class(new LSClass("Array"));

template <>
inline LSArray<LSValue*>::~LSArray() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
}
template <typename T>
LSArray<T>::~LSArray() {}

template <>
inline void LSArray<LSValue*>::push_clone(LSValue* value) {
	this->push_back(value->clone_inc());
}
template <typename T>
void LSArray<T>::push_clone(T value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_move(LSValue* value) {
	this->push_back(value->move_inc());
}
template <typename T>
void LSArray<T>::push_move(T value) {
	this->push_back(value);
}

template <>
inline void LSArray<LSValue*>::push_inc(LSValue* value) {
	if (!value->native) value->refs++;
	this->push_back(value);
}
template <>
inline void LSArray<int>::push_inc(int value) {
	this->push_back(value);
}
template <>
inline void LSArray<double>::push_inc(double value) {
	this->push_back(value);
}

template <class T>
LSArray<T>::LSArray() {}

template <class T>
LSArray<T>::LSArray(std::initializer_list<T> values_list) {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <class T>
LSArray<T>::LSArray(const std::vector<T>& vec) : LSValue(), std::vector<T>(vec) {}

template <>
inline LSArray<LSValue*>::LSArray(const LSArray<LSValue*>& other) : LSValue(other), std::vector<LSValue*>() {
	reserve(other.size());
	for (LSValue* v : other) {
		push_back(v->clone_inc());
	}
}
template <typename T>
inline LSArray<T>::LSArray(const LSArray<T>& other) : LSValue(other), std::vector<T>(other) {
}

template <class T>
LSArray<T>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone((T) LSValue::parse(it.value()));
	}
}

template <>
inline LSArray<int>::LSArray(Json& json) {
	for (Json::iterator it = json.begin(); it != json.end(); ++it) {
		push_clone(((LSNumber*) LSValue::parse(it.value()))->value);
	}
}



template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_clear() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
	this->clear();
	return this;
}
template <class T>
inline LSArray<T>* LSArray<T>::ls_clear() {
	this->clear();
	return this;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_remove(int index) {
	LSValue* previous = this->operator [] (index);
	this->erase(this->begin() + index);
	previous->refs--;
	if (refs == 0) delete this;
	return previous;
}
template <typename T>
inline T LSArray<T>::ls_remove(int index) {
	T previous = this->operator [] (index);
	this->erase(this->begin() + index);
	if (refs == 0) delete this;
	return previous;
}

template <>
inline bool LSArray<LSValue*>::ls_remove_element(LSValue* element) {
	for (size_t i = 0; i < this->size(); ++i) {
		if (*(*this)[i] == *element) {
			LSValue::delete_temporary(element);
			LSValue::delete_ref(this->operator[] (i));
			(*this)[i] = this->back();
			this->pop_back();
			if (refs == 0) delete this;
			return true;
		}
	}
	LSValue::delete_temporary(element);
	if (refs == 0) delete this;
	return false;
}
template <typename T>
inline bool LSArray<T>::ls_remove_element(T element) {
	for (size_t i = 0; i < this->size(); ++i) {
		if ((*this)[i] == element) {
			(*this)[i] = this->back();
			this->pop_back();
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}



template <>
inline LSValue* LSArray<LSValue*>::ls_sum() {
	if (this->size() == 0) return LSNumber::get(0);
	LSValue* sum = this->operator [] (0)->clone();
	for (size_t i = 1; i < this->size(); ++i) {
		sum = sum->ls_add((*this)[i]);
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline int LSArray<int>::ls_sum() {
	int sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSArray<double>::ls_sum() {
	double sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSArray<LSValue*>::ls_average() {
	if (refs == 0) delete this;
	return 0; // No average for a no integer array
}

template <>
inline double LSArray<double>::ls_average() {
	if (size() == 0) return 0;
	return this->ls_sum() / size();
}

template <>
inline double LSArray<int>::ls_average() {
	if (size() == 0) return 0;
	return (double) this->ls_sum() / size();
}

template <>
inline LSValue* LSArray<LSValue*>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* first = front();
	if (refs == 0) {
		if (first->refs == 1) {
			(*this)[0] = nullptr;
			first->refs = 0;
		}
		delete this;
		// In that case `first` will survive
	}
	return first->move(); /* return temporary */
}
template <>
inline LSValue* LSArray<double>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(first);
}
template <>
inline LSValue* LSArray<int>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(first);
}

template <>
inline LSValue* LSArray<LSValue*>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = back();
	if (refs == 0) {
		if (last->refs == 1) {
			pop_back();
			last->refs = 0;
		}
		delete this;
	}
	return last->move();
}
template <>
inline LSValue* LSArray<double>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(last);
}
template <>
inline LSValue* LSArray<int>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return LSNumber::get(last);
}

template <typename T>
inline bool LSArray<T>::ls_empty() {
	bool e = this->empty();
	if (refs == 0) delete this;
	return e;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = back();
	last->refs--;
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last->move();
}
template <>
inline LSValue* LSArray<int>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = LSNumber::get(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}
template <>
inline LSValue* LSArray<double>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return LSNull::get();
	}
	LSValue* last = LSNumber::get(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}

template <typename T>
inline int LSArray<T>::ls_size() {
	int s = this->size();
	if (refs == 0) {
		delete this;
	}
	return s;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map(const void* function) {

	auto fun = (LSValue* (*)(void*)) function;

	if (refs == 0) {
		/* In that case we have the abolute ownership of `this`
		 * Then instead of commit sucide, let optimize and work inplace
		 */
		for (size_t i = 0; i < size(); ++i) {
			LSValue* v = fun((*this)[i]);
			LSValue::delete_ref((*this)[i]);
			(*this)[i] = v->move_inc();
		}
		return this;
	} else {
		LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
		new_array->reserve(size());
		for (auto v : *this) {
			LSValue* c = v->clone();
			new_array->push_move(fun(c));
		}
		return new_array;
	}
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map(const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(int)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) {
		/* No other possibilities than sucide
		 * We have to delete temporary arguments that are not returned
		 */
		delete this;
	}
	return new_array;
}
template <>
inline LSArray<LSValue*>* LSArray<double>::ls_map(const void* function) {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(double)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSArray<double>* LSArray<int>::ls_map_real(const void* function) {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(int)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}
template <>
inline LSArray<double>* LSArray<double>::ls_map_real(const void* function) {
	LSArray<double>* new_array = new LSArray<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(double)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSArray<int>* LSArray<int>::ls_map_int(const void* function) {
	auto fun = (int (*)(int)) function;

	if (refs == 0) {
		for (size_t i = 0; i < size(); ++i) {
			(*this)[i] = fun((*this)[i]);
		}
		return this;
	} else {
		LSArray<int>* new_array = new LSArray<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			new_array->push_back(fun(v));
		}
		return new_array;
	}
}
template <>
inline LSArray<int>* LSArray<double>::ls_map_int(const void* function) {
	LSArray<int>* new_array = new LSArray<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(double)) function;
	for (auto v : *this) {
		new_array->push_inc(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}



template <typename T>
inline LSArray<LSValue*>* LSArray<T>::ls_chunk(int size) {
	if (size <= 0) size = 1;

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();

	new_array->reserve(this->size() / size + 1);

	size_t i = 0;
	while (i < this->size()) {
		LSArray<T>* sub_array = new LSArray<T>();
		sub_array->reserve(size);

		size_t j = std::min(i + size, this->size());
		if (refs == 0) {
			for (; i < j; ++i) {
				sub_array->push_inc((*this)[i]);
			}
		} else {
			for (; i < j; ++i) {
				sub_array->push_clone((*this)[i]);
			}
		}

		new_array->push_inc(sub_array);
	}
	if (refs == 0) {
		delete this;
	}
	return new_array;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_unique() {
	if (this->empty()) return this;

	auto it = this->begin();
	auto next = it;

	while (true) {
		++next;
		while (next != this->end() && (**next) == (**it)) {
			LSValue::delete_ref(*next);
			next++;
		}
		++it;
		if (next == this->end()) {
			break;
		}
		*it = *next;
	}
	this->resize(std::distance(this->begin(), it));
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_sort() {
	std::sort(this->begin(), this->end(), [](LSValue* a, LSValue* b) -> bool {
		return *a < *b;
	});
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}

template <class T>
void LSArray<T>::ls_iter(const void* function) {

	auto fun = (void* (*)(T)) function;

	for (auto v : *this) {
		fun(v);
	}

	if (refs == 0) delete this;
}

template <>
inline bool LSArray<LSValue*>::ls_contains(LSValue* val) {
	for (auto v : *this) {
		if (*v == *val) {
			if (refs == 0) delete this;
			if (val->refs == 0) delete val;
			return true;
		}
	}
	if (refs == 0) delete this;
	if (val->refs == 0) delete val;
	return false;
}
template <>
inline bool LSArray<double>::ls_contains(double val) {
	for (auto v : *this) {
		if (v == val) {
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}
template <>
inline bool LSArray<int>::ls_contains(int val) {
	for (auto v : *this) {
		if (v == val) {
			if (refs == 0) delete this;
			return true;
		}
	}
	if (refs == 0) delete this;
	return false;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_push(LSValue* val) {
	this->push_back(val->move_inc());
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_push(int val) {
	this->push_back(val);
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_push(double val) {
	this->push_back(val);
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_ptr(LSArray<LSValue*>* array) {

	this->reserve(this->size() + array->size());

	if (array->refs == 0) {
		for (LSValue* v : *array) {
			this->push_back(v);
		}
		array->clear();
		delete array;
	} else {
		for (LSValue* v : *array) {
			this->push_clone(v);
		}
	}

	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_ptr(LSArray<LSValue*>* array) {

	this->reserve(this->size() + array->size());

	for (LSValue* v : *array) {
		if (LSNumber* n = dynamic_cast<LSNumber*>(v)) {
			this->push_back(n->value);
		}
	}

	if (array->refs == 0) delete array;
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_int(LSArray<int>* array) {

	this->reserve(this->size() + array->size());

	for (int v : *array) {
		this->push_inc(LSNumber::get(v));
	}

	if (array->refs == 0) delete array;
	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_int(LSArray<int>* array) {

	this->reserve(this->size() + array->size());
	this->insert(this->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;
	return this;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_push_all_flo(LSArray<double>* array) {

	this->reserve(this->size() + array->size());

	for (double v : *array) {
		this->push_inc(LSNumber::get(v));
	}

	if (array->refs == 0) delete array;
	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_push_all_flo(LSArray<double>* array) {

	this->reserve(this->size() + array->size());
	this->insert(this->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;
	return this;
}


template <typename T>
inline LSArray<T>* LSArray<T>::ls_shuffle() {

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ++i) {
			size_t j = rand() % this->size();
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j];
			(*this)[j] = tmp;
		}
		return this;
	} else {
		LSArray<T>* new_array = new LSArray<T>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			new_array->push_clone(v);
		}
		for (size_t i = 0; i < new_array->size(); ++i) {
			size_t j = rand() % new_array->size();
			T tmp = (*new_array)[i];
			(*new_array)[i] = (*new_array)[j];
			(*new_array)[j] = tmp;
		}
		return new_array;
	}
}

template <typename T>
inline LSArray<T>* LSArray<T>::ls_reverse() {

	if (refs == 0) {
		for (size_t i = 0, j = this->size(); i < j; ++i, --j) {
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j-1];
			(*this)[j-1] = tmp;
		}
		return this;
	} else {
		LSArray<T>* new_array = new LSArray<T>();
		new_array->reserve(this->size());
		for (auto it = this->rbegin(); it != this->rend(); it++) {
			new_array->push_clone(*it);
		}
		return new_array;
	}
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_filter(const void* function) {
	auto fun = (bool (*)(void*)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			LSValue* v = (*this)[i];
			if (!fun(v)) {
				LSValue::delete_ref(v);
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSArray<double>* LSArray<double>::ls_filter(const void* function) {
	auto fun = (bool (*)(double)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			double v = (*this)[i];
			if (!fun(v)) {
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<double>* new_array = new LSArray<double>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSArray<int>* LSArray<int>::ls_filter(const void* function) {
	auto fun = (bool (*)(int)) function;

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ) {
			int v = (*this)[i];
			if (!fun(v)) {
				(*this)[i] = this->back();
				this->pop_back();
			} else {
				++i;
			}
		}
		return this;
	} else {
		LSArray<int>* new_array = new LSArray<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}


template <>
inline LSValue* LSArray<LSValue*>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<int>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<double>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<int>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSArray<double>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}



template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_insert(LSValue* value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, LSNull::get());
	}

	this->insert(this->begin() + pos, value->move_inc());

	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_insert(T value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, (T) 0);
	}

	this->insert(this->begin() + pos, value);

	return this;
}


template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_partition(const void* function) {

	LSArray<LSValue*>* array_true = new LSArray<LSValue*>();
	LSArray<LSValue*>* array_false = new LSArray<LSValue*>();
	auto fun = (bool (*)(void*)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_clone(v);
		} else {
			array_false->push_clone(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}
template <>
inline LSArray<LSValue*>* LSArray<double>::ls_partition(const void* function) {

	LSArray<double>* array_true = new LSArray<double>();
	LSArray<double>* array_false = new LSArray<double>();
	auto fun = (bool (*)(double)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_back(v);
		} else {
			array_false->push_back(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}
template <>
inline LSArray<LSValue*>* LSArray<int>::ls_partition(const void* function) {

	LSArray<int>* array_true = new LSArray<int>();
	LSArray<int>* array_false = new LSArray<int>();
	auto fun = (bool (*)(int)) function;

	for (auto v : *this) {
		if (fun(v)) {
			array_true->push_back(v);
		} else {
			array_false->push_back(v);
		}
	}
	if (refs == 0) delete this;
	array_true->refs = 1;
	array_false->refs = 1;
	return new LSArray<LSValue*> {array_true, array_false};
}


template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map2(LSArray<LSValue*>* array, const void* function) {
	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		LSValue* v2 = ((LSArray<LSValue*>*) array)->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map2(LSArray<LSValue*>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = LSNumber::get(this->operator [] (i));
		LSValue* v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_map2_int(LSArray<int>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSArray<LSValue*>* LSArray<int>::ls_map2_int(LSArray<int>* array, const void* function) {

	LSArray<LSValue*>* new_array = new LSArray<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(int, int)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		int v1 = this->operator [] (i);
		int v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline int LSArray<LSValue*>::ls_search(LSValue* needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (*needle == *(*this)[i]) {
			if (refs == 0) delete this;
			LSValue::delete_temporary(needle);
			return i;
		}
	}
	if (refs == 0) delete this;
	LSValue::delete_temporary(needle);
	return -1;
}
template <>
inline int LSArray<double>::ls_search(double needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (needle == (*this)[i]) {
			if (refs == 0) delete this;
			return i;
		}
	}
	if (refs == 0) delete this;
	return -1;
}
template <>
inline int LSArray<int>::ls_search(int needle, int start) {

	for (size_t i = start; i < this->size(); i++) {
		if (needle == (*this)[i]) {
			if (refs == 0) delete this;
			return i;
		}
	}
	if (refs == 0) delete this;
	return -1;
}

template <>
inline LSString* LSArray<LSValue*>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	glue->refs++; // because we will use it several times
	auto it = this->begin();
	LSValue* result = new LSString();
	result = result->ls_add(*it);
	for (++it; it != this->end(); ++it) {
		result = result->ls_add(glue);
		result = result->ls_add(*it);
	}
	glue->refs--;
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return (LSString*) result;
}
template <>
inline LSString* LSArray<int>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	size_t i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = result + *glue + std::to_string(this->operator[] (i));
	}
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return new LSString(result);
}
template <>
inline LSString* LSArray<double>::ls_join(LSString* glue) {
	if (this->empty()) {
		if (refs == 0) delete this;
		if (glue->refs == 0) delete glue;
		return new LSString();
	}
	size_t i = 0;
	std::string result = std::to_string(this->operator[] (i));
	for (i++; i < this->size(); i++) {
		result = result + *glue + std::to_string(this->operator[] (i));
	}
	if (refs == 0) delete this;
	if (glue->refs == 0) delete glue;
	return new LSString(result);
}

template <>
inline LSArray<LSValue*>* LSArray<LSValue*>::ls_fill(LSValue* element, int size) {
	this->clear();
	this->reserve(size);
	for (int i = 0; i < size; i++) {
		this->push_move(element);
	}
	LSValue::delete_temporary(element); // only useful if size = 0
	return this;
}
template <typename T>
inline LSArray<T>* LSArray<T>::ls_fill(T element, int size) {
	this->clear();
	this->resize(size, element);
	return this;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_max() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return LSNull::get();
	}

	LSValue* max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if (*(*this)[i] < *max) {
			max = (*this)[i];
		}
	}
	if (refs == 0) {
		max = max->clone();
		delete this;
	}
	return max;
}
template <typename T>
inline T LSArray<T>::ls_max() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return (T) 0;
	}

	T max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if (max < (*this)[i]) {
			max = (*this)[i];
		}
	}
	if (refs == 0) delete this;
	return max;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_min() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return LSNull::get();
	}

	LSValue* max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if (*max < *(*this)[i]) {
			max = (*this)[i];
		}
	}
	if (refs == 0) {
		max = max->clone();
		delete this;
	}
	return max;
}
template <typename T>
inline T LSArray<T>::ls_min() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return (T) 0;
	}

	T max = (*this)[0];
	for (size_t i = 1; i < this->size(); ++i) {
		if ((*this)[i] < max) {
			max = (*this)[i];
		}
	}
	if (refs == 0) delete this;
	return max;
}



/*
 * LSValue methods
 */
template <class T>
bool LSArray<T>::isTrue() const {
	return this->size() > 0;
}

template <class T>
LSValue* LSArray<T>::ls_not() {
	bool r = this->size() == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

template <class T>
LSValue* LSArray<T>::ls_tilde() {
	LSArray<T>* array = new LSArray<T>();
	array->reserve(this->size());
	if (refs == 0) {
		for (auto i = this->rbegin(); i != this->rend(); ++i) {
			array->push_back(*i);
		}
		this->clear();
		delete this;
	} else {
		for (auto i = this->rbegin(); i != this->rend(); ++i) {
			array->push_clone(*i);
		}
	}
	return array;
}





template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSNull* v) {
	if (refs == 0) {
		push_back(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_back(v);
	return new_array;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSNull* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSBoolean* v) {
	if (refs == 0) {
		push_back(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_back(v);
	return new_array;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSBoolean* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSNumber* v) {
	if (refs == 0) {
		push_move(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}
template <>
inline LSValue* LSArray<double>::ls_add(LSNumber* v) {
	if (refs == 0) {
		this->push_back(v->value);
		if (v->refs == 0) delete v;
		return this;
	}
	LSArray<double>* r = (LSArray<double>*) this->clone();
	r->push_back(v->value);
	if (v->refs == 0) delete v;
	return r;
}
template <>
inline LSValue* LSArray<int>::ls_add(LSNumber* v) {
	if (v->value == (int) v->value) {
		if (refs == 0) {
			this->push_back(v->value);
			if (v->refs == 0) delete v;
			return this;
		}
		LSArray<double>* r = (LSArray<double>*) this->clone();
		r->push_back(v->value);
		if (v->refs == 0) delete v;
		return r;
	}
	LSArray<double>* ret = new LSArray<double>();
	ret->insert(ret->end(), this->begin(), this->end());
	ret->push_back(v->value);
	if (refs == 0) delete this;
	if (v->refs == 0) delete v;
	return ret;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSString* v) {
	if (refs == 0) {
		this->push_move(v);
		return this;
	}
	LSArray<LSValue*>* r = (LSArray<LSValue*>*) this->clone();
	r->push_move(v);
	return r;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSString* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSArray<LSValue*>* array) {
	if (refs == 0) {
		return ls_push_all_ptr(array);
	}
	return ((LSArray<LSValue*>*) this->clone())->ls_push_all_ptr(array);
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSArray<LSValue*>* array) {
	LSArray<LSValue*>* ret = new LSArray<LSValue*>();
	ret->reserve(this->size() + array->size());

	for (T v : *this) {
		ret->push_inc(LSNumber::get(v));
	}

	if (array->refs == 0) {
		for (LSValue* v : *array) {
			ret->push_back(v); // steal the ownership
		}
		array->clear();
		delete array;
	} else {
		for (LSValue* v : *array) {
			ret->push_clone(v);
		}
	}

	if (refs == 0) delete this;
	return ret;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSArray<int>* array) {
	if (refs == 0) {
		return ls_push_all_int(array);
	}
	return ((LSArray<T>*) this->clone())->ls_push_all_int(array);
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSArray<double>* array) {
	if (refs == 0) {
		return ls_push_all_flo(array);
	}
	return ((LSArray<T>*) this->clone())->ls_push_all_flo(array);
}
template <>
inline LSValue* LSArray<int>::ls_add(LSArray<double>* array) {
	LSArray<double>* ret = new LSArray<double>();
	ret->reserve(this->size() + array->size());

	ret->insert(ret->end(), this->begin(), this->end());
	ret->insert(ret->end(), array->begin(), array->end());

	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return ret;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSObject* v) {
	if (refs == 0) {
		this->push_move(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSObject* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSFunction* v) {
	if (refs == 0) {
		push_move(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSFunction* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add(LSClass* v) {
	if (refs == 0) {
		push_move(v);
		return this;
	}
	LSArray<LSValue*>* new_array = (LSArray<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add(LSClass* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}


/* let array = []
 * array += [1,2,3,4]
 */

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSNull* v) {
	push_back(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSNull* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSBoolean* v) {
	push_back(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSBoolean* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSNumber* v) {
	push_move(v);
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_add_eq(LSNumber* v) {
	this->push_back(v->value);
	if (v->refs == 0) delete v;
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_add_eq(LSNumber* v) {
	if (v->value == (int) v->value) {
		this->push_back(v->value);
		if (v->refs == 0) delete v;
		return this;
	}
	LSArray<double>* r = new LSArray<double>();
	r->insert(r->end(), this->begin(), this->end());
	r->push_back(v->value);
	if (v->refs == 0) delete v;
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSString* v) {
	this->push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSString* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSArray<LSValue*>* array) {
	ls_push_all_ptr(array);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSArray<LSValue*>* array) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + array->size());

	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}

	if (array->refs == 0) {
		for (LSValue* v : *array) {
			r->push_back(v); // steal the ownership
		}
		array->clear();
		delete array;
	} else {
		for (LSValue* v : *array) {
			r->push_clone(v);
		}
	}

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSArray<int>* array) {
	return ls_push_all_int(array);
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSArray<double>* array) {
	return ls_push_all_flo(array);
}
template <>
inline LSValue* LSArray<int>::ls_add_eq(LSArray<double>* array) {
	LSArray<double>* r = new LSArray<double>();
	r->reserve(this->size() + array->size());

	r->insert(r->end(), this->begin(), this->end());
	r->insert(r->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSSet<LSValue*>* set) {
	this->reserve(this->size() + set->size());

	if (set->refs == 0) {
		for (LSValue* v : *set) {
			this->push_back(v);
		}
		set->clear();
		delete set;
	} else {
		for (LSValue* v : *set) {
			this->push_clone(v);
		}
	}
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSSet<LSValue*>* set) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	r->reserve(this->size() + set->size());

	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}

	if (set->refs == 0) {
		for (LSValue* v : *set) {
			r->push_back(v); // steal the ownership
		}
		set->clear();
		delete set;
	} else {
		for (LSValue* v : *set) {
			r->push_clone(v);
		}
	}

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}
template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSSet<int>* set) {
	this->reserve(this->size() + set->size());

	for (int v : *set) {
		this->push_inc(LSNumber::get(v));
	}
	if (set->refs == 0) delete set;
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSSet<int>* set) {
	this->reserve(this->size() + set->size());
	this->insert(this->end(), set->begin(), set->end());
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSSet<double>* set) {
	this->reserve(this->size() + set->size());

	for (int v : *set) {
		this->push_inc(LSNumber::get(v));
	}
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSArray<double>::ls_add_eq(LSSet<double>* set) {
	this->reserve(this->size() + set->size());
	this->insert(this->end(), set->begin(), set->end());
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSArray<int>::ls_add_eq(LSSet<double>* set) {
	LSArray<double>* r = new LSArray<double>();
	r->reserve(this->size() + set->size());

	r->insert(r->end(), this->begin(), this->end());
	r->insert(r->end(), set->begin(), set->end());

	if (set->refs == 0) delete set;

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}


template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSObject* v) {
	this->push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSObject* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSFunction* v) {
	push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSFunction* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_move(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSArray<LSValue*>::ls_add_eq(LSClass* v) {
	push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSArray<T>::ls_add_eq(LSClass* v) {
	LSArray<LSValue*>* r = new LSArray<LSValue*>();
	for (T v : *this) {
		r->push_inc(LSNumber::get(v));
	}
	r->push_back(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline bool LSArray<LSValue*>::eq(const LSArray<LSValue*>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		if (**i != **j) return false;
	}
	return true;
}

template <typename T>
inline bool LSArray<T>::eq(const LSArray<LSValue*>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*j);
		if (!n) return false;
		if (n->value != *i) return false;
	}
	return true;
}

template <>
inline bool LSArray<LSValue*>::eq(const LSArray<int>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*i);
		if (!n) return false;
		if (n->value != *j) return false;
	}
	return true;
}

template <typename T>
inline bool LSArray<T>::eq(const LSArray<int>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		if (*i != *j) return false;
	}
	return true;
}

template <>
inline bool LSArray<LSValue*>::eq(const LSArray<double>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		const LSNumber* n = dynamic_cast<const LSNumber*>(*i);
		if (!n) return false;
		if (n->value != *j) return false;
	}
	return true;
}

template <typename T>
inline bool LSArray<T>::eq(const LSArray<double>* array) const {

	if (this->size() != array->size()) {
		return false;
	}
	auto i = this->begin();
	auto j = array->begin();

	for (; i != this->end(); i++, j++) {
		if (*i != *j) return false;
	}
	return true;
}

template <>
inline bool LSArray<LSValue*>::lt(const LSArray<LSValue*>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end(), [](const LSValue* a, const LSValue* b) -> bool {
		return *a < *b;
	});
}
template <typename T>
inline bool LSArray<T>::lt(const LSArray<LSValue*>* v) const {
	auto i = this->begin();
	auto j = v->begin();
	while (i != this->end()) {
		if (j == v->end()) return false;
		if ((*j)->typeID() < 3) return false;
		if (3 < (*j)->typeID()) return true;
		if (*i < ((LSNumber*) *j)->value) return true;
		if (((LSNumber*) *j)->value < *i) return false;
		++i; ++j;
	}
	return (j != v->end());
}

template <>
inline bool LSArray<LSValue*>::lt(const LSArray<int>* v) const {
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
inline bool LSArray<T>::lt(const LSArray<int>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}

template <>
inline bool LSArray<LSValue*>::lt(const LSArray<double>* v) const {
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
inline bool LSArray<T>::lt(const LSArray<double>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}



template <>
inline bool LSArray<int>::in(LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline bool LSArray<double>::in(LSValue* key) const {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		for (auto i = this->begin(); i != this->end(); i++) {
			if ((*i) == n->value) {
				return true;
			}
		}
	}
	return false;
}

template <>
inline bool LSArray<LSValue*>::in(LSValue* key) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if (**i == *key) {
			return true;
		}
	}
	return false;
}

template <>
inline int LSArray<int>::atv(const int i) {
	return this->operator[] (i);
}

template <>
inline int* LSArray<int>::atLv(int i) {
	return &this->operator[] (i);
}

template <typename T>
inline LSValue* LSArray<T>::range(int start, int end) const {

	LSArray<T>* range = new LSArray<T>();

	size_t start_i = std::max<size_t>(0, start);
	size_t end_i = std::min<size_t>(this->size(), end);

	for (size_t i = start_i; i < end_i; ++i) {
		range->push_clone(this->operator [] (i));
	}
	return range;
}

template <class T>
LSValue* LSArray<T>::rangeL(int, int) {
	return this;
}

template <class T>
LSValue* LSArray<T>::clone() const {
	return new LSArray<T>(*this);
}

template <typename T>
std::ostream& LSArray<T>::print(std::ostream& os) const {

	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << **i;
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<int>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <>
inline std::ostream& LSArray<double>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << (*i);
	}
	os << "]";
	return os;
}

template <typename T>
std::string LSArray<T>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::string json = (*i)->to_json();
		res += json;
	}
	return res + "]";
}

template <>
inline std::string LSArray<int>::json() const {
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
inline std::string LSArray<double>::json() const {
	std::string res = "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) res += ",";
		std::ostringstream oss;
		oss << *i;
		res += oss.str();
	}
	return res + "]";
}

template <class T>
LSValue* LSArray<T>::getClass() const {
	return LSArray<T>::array_class;
}

template <class T>
const BaseRawType* LSArray<T>::getRawType() const {
	return RawType::ARRAY;
}

template <class T>
LSValue* LSArray<T>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return (LSValue*) ((std::vector<T>*) this)->at((int) n->value)->clone();
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<int>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<int>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <>
inline LSValue* LSArray<double>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return LSNumber::get(((std::vector<double>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return LSNull::get();
		}
	}
	return LSNull::get();
}

template <class T>
LSValue* LSArray<T>::attr(const LSValue* key) const {

	if (*((LSString*) key) == "size") {
		return LSNumber::get(this->size());
	}
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}


template <class T>
LSValue** LSArray<T>::atL(const LSValue* key) {
	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			LSValue** v = (LSValue**) &(((std::vector<T>*)this)->at((int) n->value));
			return v;
		} catch (std::exception& e) {
			return nullptr;
		}
	}
	return nullptr;
}

template <class T>
LSValue** LSArray<T>::attrL(const LSValue*) {
	return nullptr;
}

template <class T>
LSValue* LSArray<T>::abso() const {
	return LSNumber::get(this->size());
}

} // end of namespace ls

#endif

