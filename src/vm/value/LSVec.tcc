#ifndef LS_ARRAY_TCC
#define LS_ARRAY_TCC

#include "../LSValue.hpp"
#include "LSVar.hpp"

#include <algorithm>

namespace ls {

template <>
inline LSVec<LSValue*>::~LSVec() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
}
template <typename T>
LSVec<T>::~LSVec() {}

template <>
inline void LSVec<LSValue*>::push_clone(LSValue* value) {
	this->push_back(value->clone_inc());
}
template <typename T>
void LSVec<T>::push_clone(T value) {
	this->push_back(value);
}

template <>
inline void LSVec<LSValue*>::push_move(LSValue* value) {
	this->push_back(value->move_inc());
}
template <typename T>
void LSVec<T>::push_move(T value) {
	this->push_back(value);
}

template <>
inline void LSVec<LSValue*>::push_inc(LSValue* value) {
	value->refs++;
	this->push_back(value);
}
template <>
inline void LSVec<int>::push_inc(int value) {
	this->push_back(value);
}
template <>
inline void LSVec<double>::push_inc(double value) {
	this->push_back(value);
}

template <class T>
LSVec<T>::LSVec() {}

template <class T>
LSVec<T>::LSVec(std::initializer_list<T> values_list) {
	for (auto i : values_list) {
		this->push_back(i);
	}
}

template <class T>
LSVec<T>::LSVec(const std::vector<T>& vec) : LSValue(), std::vector<T>(vec) {}

template <>
inline LSVec<LSValue*>::LSVec(const LSVec<LSValue*>& other) : LSValue(other), std::vector<LSValue*>() {
	reserve(other.size());
	for (LSValue* v : other) {
		push_back(v->clone_inc());
	}
}
template <typename T>
inline LSVec<T>::LSVec(const LSVec<T>& other) : LSValue(other), std::vector<T>(other) {
}

template <class T>
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
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_clear() {
	for (auto v : *this) {
		LSValue::delete_ref(v);
	}
	this->clear();
	return this;
}
template <class T>
inline LSVec<T>* LSVec<T>::ls_clear() {
	this->clear();
	return this;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_remove(int index) {
	LSValue* previous = this->operator [] (index);
	this->erase(this->begin() + index);
	previous->refs--;
	if (refs == 0) delete this;
	return previous;
}
template <typename T>
inline T LSVec<T>::ls_remove(int index) {
	T previous = this->operator [] (index);
	this->erase(this->begin() + index);
	if (refs == 0) delete this;
	return previous;
}

template <>
inline bool LSVec<LSValue*>::ls_remove_element(LSValue* element) {
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
inline bool LSVec<T>::ls_remove_element(T element) {
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


/*
template <>
inline LSValue* LSVec<LSValue*>::ls_sum() {
	if (this->size() == 0) return new LSVar(0);
	LSValue* sum = this->operator [] (0)->clone();
	for (size_t i = 1; i < this->size(); ++i) {
		sum = sum->ls_add((*this)[i]);
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline int LSVec<int>::ls_sum() {
	int sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSVec<double>::ls_sum() {
	double sum = 0;
	for (auto v : *this) {
		sum += v;
	}
	if (refs == 0) delete this;
	return sum;
}

template <>
inline double LSVec<LSValue*>::ls_average() {
	if (refs == 0) delete this;
	return 0; // No average for a no integer array
}

template <>
inline double LSVec<double>::ls_average() {
	if (size() == 0) return 0;
	return this->ls_sum() / size();
}

template <>
inline double LSVec<int>::ls_average() {
	if (size() == 0) return 0;
	return (double) this->ls_sum() / size();
}
*/

template <>
inline LSValue* LSVec<LSValue*>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
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
inline LSValue* LSVec<double>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return new LSVar(first);
}
template <>
inline LSValue* LSVec<int>::ls_first() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	double first = front();
	if (refs == 0) {
		delete this;
	}
	return new LSVar(first);
}

template <>
inline LSValue* LSVec<LSValue*>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
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
inline LSValue* LSVec<double>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return new LSVar(last);
}
template <>
inline LSValue* LSVec<int>::ls_last() {
	if (this->size() == 0) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	double last = back();
	if (refs == 0) {
		delete this;
	}
	return new LSVar(last);
}

template <typename T>
inline bool LSVec<T>::ls_empty() {
	bool e = this->empty();
	if (refs == 0) delete this;
	return e;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
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
inline LSValue* LSVec<int>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	LSValue* last = new LSVar(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}
template <>
inline LSValue* LSVec<double>::ls_pop() {
	if (empty()) {
		if (refs == 0) {
			delete this;
		}
		return new LSVar();
	}
	LSValue* last = new LSVar(back());
	pop_back();
	if (refs == 0) {
		delete this;
	}
	return last;
}

template <typename T>
inline int LSVec<T>::ls_size() {
	int s = this->size();
	if (refs == 0) {
		delete this;
	}
	return s;
}

/*
template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_map(const void* function) {

	auto fun = (LSValue* (*)(void*)) function;

	if (refs == 0) {
		// In that case we have the abolute ownership of `this`
		// Then instead of commit sucide, let optimize and work inplace

		for (size_t i = 0; i < size(); ++i) {
			LSValue* v = fun((*this)[i]);
			LSValue::delete_ref((*this)[i]);
			(*this)[i] = v->move_inc();
		}
		return this;
	} else {
		LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
		new_array->reserve(size());
		for (auto v : *this) {
			LSValue* c = v->clone();
			new_array->push_move(fun(c));
		}
		return new_array;
	}
}

template <>
inline LSVec<LSValue*>* LSVec<int>::ls_map(const void* function) {

	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(int)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) {
		// No other possibilities than suicide
		// We have to delete temporary arguments that are not returned

		delete this;
	}
	return new_array;
}
template <>
inline LSVec<LSValue*>* LSVec<double>::ls_map(const void* function) {
	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (LSValue* (*)(double)) function;
	for (auto v : *this) {
		new_array->push_move(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSVec<double>* LSVec<int>::ls_map_real(const void* function) {
	LSVec<double>* new_array = new LSVec<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(int)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}
template <>
inline LSVec<double>* LSVec<double>::ls_map_real(const void* function) {
	LSVec<double>* new_array = new LSVec<double>();
	new_array->reserve(this->size());
	auto fun = (double (*)(double)) function;
	for (auto v : *this) {
		new_array->push_back(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}

template <>
inline LSVec<int>* LSVec<int>::ls_map_int(const void* function) {
	auto fun = (int (*)(int)) function;

	if (refs == 0) {
		for (size_t i = 0; i < size(); ++i) {
			(*this)[i] = fun((*this)[i]);
		}
		return this;
	} else {
		LSVec<int>* new_array = new LSVec<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			new_array->push_back(fun(v));
		}
		return new_array;
	}
}
template <>
inline LSVec<int>* LSVec<double>::ls_map_int(const void* function) {
	LSVec<int>* new_array = new LSVec<int>();
	new_array->reserve(this->size());
	auto fun = (int (*)(double)) function;
	for (auto v : *this) {
		new_array->push_inc(fun(v));
	}
	if (refs == 0) delete this;
	return new_array;
}



template <typename T>
inline LSVec<LSValue*>* LSVec<T>::ls_chunk(int size) {
	if (size <= 0) size = 1;

	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();

	new_array->reserve(this->size() / size + 1);

	size_t i = 0;
	while (i < this->size()) {
		LSVec<T>* sub_array = new LSVec<T>();
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
inline LSValue* LSVec<LSValue*>::ls_unique() {
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
inline LSValue* LSVec<int>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}
template <>
inline LSValue* LSVec<double>::ls_unique() {
	auto it = std::unique(this->begin(), this->end());
	this->resize(std::distance(this->begin(), it));
	return this;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_sort() {
	std::sort(this->begin(), this->end(), [](LSValue* a, LSValue* b) -> bool {
		return *a < *b;
	});
	return this;
}
template <>
inline LSValue* LSVec<int>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}
template <>
inline LSValue* LSVec<double>::ls_sort() {
	std::sort(this->begin(), this->end());
	return this;
}

template <class T>
void LSVec<T>::ls_iter(const void* function) {

	auto fun = (void* (*)(T)) function;

	for (auto v : *this) {
		fun(v);
	}

	if (refs == 0) delete this;
}

template <>
inline bool LSVec<LSValue*>::ls_contains(LSValue* val) {
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
inline bool LSVec<double>::ls_contains(double val) {
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
inline bool LSVec<int>::ls_contains(int val) {
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
inline LSValue* LSVec<LSValue*>::ls_push(LSValue* val) {
	this->push_back(val->move_inc());
	return this;
}
template <>
inline LSValue* LSVec<int>::ls_push(int val) {
	this->push_back(val);
	return this;
}
template <>
inline LSValue* LSVec<double>::ls_push(double val) {
	this->push_back(val);
	return this;
}

template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_push_all_ptr(LSVec<LSValue*>* array) {

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
inline LSVec<T>* LSVec<T>::ls_push_all_ptr(LSVec<LSValue*>* array) {

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
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_push_all_int(LSVec<int>* array) {

	this->reserve(this->size() + array->size());

	for (int v : *array) {
		this->push_inc(new LSVar(v));
	}

	if (array->refs == 0) delete array;
	return this;
}
template <typename T>
inline LSVec<T>* LSVec<T>::ls_push_all_int(LSVec<int>* array) {

	this->reserve(this->size() + array->size());
	this->insert(this->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;
	return this;
}

template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_push_all_flo(LSVec<double>* array) {

	this->reserve(this->size() + array->size());

	for (double v : *array) {
		this->push_inc(new LSVar(v));
	}

	if (array->refs == 0) delete array;
	return this;
}
template <typename T>
inline LSVec<T>* LSVec<T>::ls_push_all_flo(LSVec<double>* array) {

	this->reserve(this->size() + array->size());
	this->insert(this->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;
	return this;
}


template <typename T>
inline LSVec<T>* LSVec<T>::ls_shuffle() {

	if (refs == 0) {
		for (size_t i = 0; i < this->size(); ++i) {
			size_t j = rand() % this->size();
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j];
			(*this)[j] = tmp;
		}
		return this;
	} else {
		LSVec<T>* new_array = new LSVec<T>();
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
inline LSVec<T>* LSVec<T>::ls_reverse() {

	if (refs == 0) {
		for (size_t i = 0, j = this->size(); i < j; ++i, --j) {
			T tmp = (*this)[i];
			(*this)[i] = (*this)[j-1];
			(*this)[j-1] = tmp;
		}
		return this;
	} else {
		LSVec<T>* new_array = new LSVec<T>();
		new_array->reserve(this->size());
		for (auto it = this->rbegin(); it != this->rend(); it++) {
			new_array->push_clone(*it);
		}
		return new_array;
	}
}

template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_filter(const void* function) {
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
		LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSVec<double>* LSVec<double>::ls_filter(const void* function) {
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
		LSVec<double>* new_array = new LSVec<double>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}
template <>
inline LSVec<int>* LSVec<int>::ls_filter(const void* function) {
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
		LSVec<int>* new_array = new LSVec<int>();
		new_array->reserve(this->size());
		for (auto v : *this) {
			if (fun(v)) new_array->push_clone(v);
		}
		return new_array;
	}
}


template <>
inline LSValue* LSVec<LSValue*>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSVec<int>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSVec<double>::ls_foldLeft(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto v : *this) {
		result = fun(result, v);
	}
	if (refs == 0) delete this;
	return result;
}


template <>
inline LSValue* LSVec<LSValue*>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, LSValue*)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSVec<int>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, int)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}
template <>
inline LSValue* LSVec<double>::ls_foldRight(const void* function, LSValue* v0) {
	auto fun = (LSValue* (*)(LSValue*, double)) function;

	LSValue* result = v0->move();
	for (auto it = this->rbegin(); it != this->rend(); it++) {
		result = fun(result, *it);
	}
	if (refs == 0) delete this;
	return result;
}



template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_insert(LSValue* value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, new LSVar());
	}

	this->insert(this->begin() + pos, value->move_inc());

	return this;
}
template <typename T>
inline LSVec<T>* LSVec<T>::ls_insert(T value, int pos) {

	if (pos >= (int) this->size()) {
		this->resize(pos, (T) 0);
	}

	this->insert(this->begin() + pos, value);

	return this;
}


template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_partition(const void* function) {

	LSVec<LSValue*>* array_true = new LSVec<LSValue*>();
	LSVec<LSValue*>* array_false = new LSVec<LSValue*>();
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
	return new LSVec<LSValue*> {array_true, array_false};
}
template <>
inline LSVec<LSValue*>* LSVec<double>::ls_partition(const void* function) {

	LSVec<double>* array_true = new LSVec<double>();
	LSVec<double>* array_false = new LSVec<double>();
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
	return new LSVec<LSValue*> {array_true, array_false};
}
template <>
inline LSVec<LSValue*>* LSVec<int>::ls_partition(const void* function) {

	LSVec<int>* array_true = new LSVec<int>();
	LSVec<int>* array_false = new LSVec<int>();
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
	return new LSVec<LSValue*> {array_true, array_false};
}


template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_map2(LSVec<LSValue*>* array, const void* function) {
	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = this->operator [] (i);
		LSValue* v2 = ((LSVec<LSValue*>*) array)->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSVec<LSValue*>* LSVec<int>::ls_map2(LSVec<LSValue*>* array, const void* function) {

	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
	new_array->reserve(this->size());
	auto fun = (void* (*)(void*, void*)) function;

	for (unsigned i = 0; i < this->size(); ++i) {
		LSValue* v1 = new LSVar(this->operator [] (i));
		LSValue* v2 = array->operator [] (i);
		LSValue* res = (LSValue*) fun(v1, v2);
		new_array->push_move(res);
	}
	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return new_array;
}

template <>
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_map2_int(LSVec<int>* array, const void* function) {

	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
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
inline LSVec<LSValue*>* LSVec<int>::ls_map2_int(LSVec<int>* array, const void* function) {

	LSVec<LSValue*>* new_array = new LSVec<LSValue*>();
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
inline int LSVec<LSValue*>::ls_search(LSValue* needle, int start) {

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
inline int LSVec<double>::ls_search(double needle, int start) {

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
inline int LSVec<int>::ls_search(int needle, int start) {

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
inline LSVec<LSValue*>* LSVec<LSValue*>::ls_fill(LSValue* element, int size) {
	this->clear();
	this->reserve(size);
	for (int i = 0; i < size; i++) {
		this->push_move(element);
	}
	LSValue::delete_temporary(element); // only useful if size = 0
	return this;
}
template <typename T>
inline LSVec<T>* LSVec<T>::ls_fill(T element, int size) {
	this->clear();
	this->resize(size, element);
	return this;
}


template <>
inline LSValue* LSVec<LSValue*>::ls_max() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return new LSVar();
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
inline T LSVec<T>::ls_max() {
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
inline LSValue* LSVec<LSValue*>::ls_min() {
	if (this->empty()) {
		if (refs == 0) delete this;
		return new LSVar();
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
inline T LSVec<T>::ls_min() {
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
*/

/*
 * LSValue methods
 */
template <class T>
bool LSVec<T>::isTrue() const {
	return this->size() > 0;
}

/*
template <class T>
LSValue* LSVec<T>::ls_not() {
	bool r = this->size() == 0;
	if (refs == 0) delete this;
	return LSBoolean::get(r);
}

template <class T>
LSValue* LSVec<T>::ls_tilde() {
	LSVec<T>* array = new LSVec<T>();
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

template <typename T>
LSValue* LSVec<T>::ls_add(LSNull* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + 1);
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSNull* v) {
	if (refs == 0) {
		push_back(v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_back(v);
	return new_array;
}

template <>
inline LSValue* LSVec<double>::ls_add(LSNull* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + 1);
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSBoolean* v) {
	if (refs == 0) {
		push_back(v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_back(v);
	return new_array;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSBoolean* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + 1);
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSNumber* v) {
	if (refs == 0) {
		push_move(v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}

template <>
inline LSValue* LSVec<double>::ls_add(LSNumber* v) {
	if (refs == 0) {
		this->push_back(v->value);
		if (v->refs == 0) delete v;
		return this;
	}
	LSVec<double>* r = (LSVec<double>*) this->clone();
	r->push_back(v->value);
	if (v->refs == 0) delete v;
	return r;
}

template <>
inline LSValue* LSVec<int>::ls_add(LSNumber* v) {
	if (v->value == (int) v->value) {
		if (refs == 0) {
			this->push_back(v->value);
			if (v->refs == 0) delete v;
			return this;
		}
		LSVec<double>* r = (LSVec<double>*) this->clone();
		r->push_back(v->value);
		if (v->refs == 0) delete v;
		return r;
	}
	LSVec<double>* ret = new LSVec<double>();
	ret->insert(ret->end(), this->begin(), this->end());
	ret->push_back(v->value);
	if (refs == 0) delete this;
	if (v->refs == 0) delete v;
	return ret;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSString* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSString* v) {
	if (refs == 0) {
		this->push_move((LSValue*) v);
		return this;
	}
	LSVec<LSValue*>* r = (LSVec<LSValue*>*) this->clone();
	r->push_move(v);
	return r;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSVec<LSValue*>* array) {
	LSVec<LSValue*>* ret = new LSVec<LSValue*>();
	ret->reserve(this->size() + array->size());

	for (auto v : *this) {
		ret->push_inc(new LSVar(v));
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

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSVec<LSValue*>* array) {
	if (refs == 0) {
		return ls_push_all_ptr(array);
	}
	return ((LSVec<LSValue*>*) this->clone())->ls_push_all_ptr(array);
}

template <typename T>
inline LSValue* LSVec<T>::ls_add(LSVec<int>* array) {
	if (refs == 0) {
		return ls_push_all_int(array);
	}
	return ((LSVec<T>*) this->clone())->ls_push_all_int(array);
}
template <typename T>
inline LSValue* LSVec<T>::ls_add(LSVec<double>* array) {
	if (refs == 0) {
		return ls_push_all_flo(array);
	}
	return ((LSVec<T>*) this->clone())->ls_push_all_flo(array);
}
template <>
inline LSValue* LSVec<int>::ls_add(LSVec<double>* array) {
	LSVec<double>* ret = new LSVec<double>();
	ret->reserve(this->size() + array->size());

	ret->insert(ret->end(), this->begin(), this->end());
	ret->insert(ret->end(), array->begin(), array->end());

	if (refs == 0) delete this;
	if (array->refs == 0) delete array;
	return ret;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSObject* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSObject* v) {
	if (refs == 0) {
		this->push_move(v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSFunction* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSFunction* v) {
	if (refs == 0) {
		push_move((LSValue*) v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}

template <typename T>
LSValue* LSVec<T>::ls_add(LSClass* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	if (refs == 0) delete this;
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add(LSClass* v) {
	if (refs == 0) {
		push_move(v);
		return this;
	}
	LSVec<LSValue*>* new_array = (LSVec<LSValue*>*) this->clone();
	new_array->push_move(v);
	return new_array;
}


template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSNull* v) {
	push_back(v);
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSNull* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSBoolean* v) {
	push_back(v);
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSBoolean* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + 1);
	for (T v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSNumber* v) {
	push_move(v);
	return this;
}
template <>
inline LSValue* LSVec<double>::ls_add_eq(LSNumber* v) {
	this->push_back(v->value);
	if (v->refs == 0) delete v;
	return this;
}
template <>
inline LSValue* LSVec<int>::ls_add_eq(LSNumber* v) {
	if (v->value == (int) v->value) {
		this->push_back(v->value);
		if (v->refs == 0) delete v;
		return this;
	}
	LSVec<double>* r = new LSVec<double>();
	r->insert(r->end(), this->begin(), this->end());
	r->push_back(v->value);
	if (v->refs == 0) delete v;
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <typename T>
LSValue* LSVec<T>::ls_add_eq(LSString* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);
	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSString* v) {
	this->push_move((LSValue*) v);
	return this;
}

template <typename T>
LSValue* LSVec<T>::ls_add_eq(LSVec<LSValue*>* array) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + array->size());

	for (auto v : *this) {
		r->push_inc(new LSVar(v));
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

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSVec<LSValue*>* array) {
	ls_push_all_ptr(array);
	return this;
}

template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSVec<int>* array) {
	return ls_push_all_int(array);
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSVec<double>* array) {
	return ls_push_all_flo(array);
}
template <>
inline LSValue* LSVec<int>::ls_add_eq(LSVec<double>* array) {
	LSVec<double>* r = new LSVec<double>();
	r->reserve(this->size() + array->size());

	r->insert(r->end(), this->begin(), this->end());
	r->insert(r->end(), array->begin(), array->end());

	if (array->refs == 0) delete array;

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}


template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSSet<LSValue*>* set) {
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
inline LSValue* LSVec<T>::ls_add_eq(LSSet<LSValue*>* set) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	r->reserve(this->size() + set->size());

	for (T v : *this) {
		r->push_inc(new LSVar(v));
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
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSSet<int>* set) {
	this->reserve(this->size() + set->size());

	for (int v : *set) {
		this->push_inc(new LSVar(v));
	}
	if (set->refs == 0) delete set;
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSSet<int>* set) {
	this->reserve(this->size() + set->size());
	this->insert(this->end(), set->begin(), set->end());
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSSet<double>* set) {
	this->reserve(this->size() + set->size());

	for (int v : *set) {
		this->push_inc(new LSVar(v));
	}
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSVec<double>::ls_add_eq(LSSet<double>* set) {
	this->reserve(this->size() + set->size());
	this->insert(this->end(), set->begin(), set->end());
	if (set->refs == 0) delete set;
	return this;
}
template <>
inline LSValue* LSVec<int>::ls_add_eq(LSSet<double>* set) {
	LSVec<double>* r = new LSVec<double>();
	r->reserve(this->size() + set->size());

	r->insert(r->end(), this->begin(), this->end());
	r->insert(r->end(), set->begin(), set->end());

	if (set->refs == 0) delete set;

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}


template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSObject* v) {
	this->push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSObject* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSFunction* v) {
	push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSFunction* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_move(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <>
inline LSValue* LSVec<LSValue*>::ls_add_eq(LSClass* v) {
	push_move(v);
	return this;
}
template <typename T>
inline LSValue* LSVec<T>::ls_add_eq(LSClass* v) {
	LSVec<LSValue*>* r = new LSVec<LSValue*>();
	for (auto v : *this) {
		r->push_inc(new LSVar(v));
	}
	r->push_back(v);

	r->refs = 1;
	LSValue::delete_ref(this);
	return r;
}

template <typename T>
bool LSVec<T>::eq(const LSVec<LSValue*>* array) const {
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
*/

/*
template <>
inline bool LSVec<LSValue*>::eq(const LSVec<LSValue*>* array) const {
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

template <>
inline bool LSVec<void*>::eq(const LSVec<int>*) const {
	return false;
}

template <typename T>
bool LSVec<T>::eq(const LSVec<int>* array) const {
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
inline bool LSVec<LSValue*>::eq(const LSVec<int>* array) const {

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

	return false;
}

/*
template <typename T>
inline bool LSVec<T>::eq(const LSVec<double>* array) const {
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
inline bool LSVec<LSValue*>::eq(const LSVec<double>* array) const {
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
inline bool LSVec<T>::lt(const LSVec<LSValue*>* v) const {
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
*/
/*
template <>
inline bool LSVec<LSValue*>::lt(const LSVec<LSValue*>* v) const {
	return std::lexicographical_compare(begin(), end(), v->begin(), v->end(), [](const LSValue* a, const LSValue* b) -> bool {
		return *a < *b;
	});
}

template <>
inline bool LSVec<void*>::lt(const LSVec<LSValue*>* v) const {
	return false;
}

template <>
inline bool LSVec<LSValue*>::lt(const LSVec<int>* v) const {
	return false;

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
template <>
inline bool LSVec<void*>::lt(const LSVec<int>* v) const {
	return false;
}
template <typename T>
inline bool LSVec<T>::lt(const LSVec<int>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}

template <>
inline bool LSVec<LSValue*>::lt(const LSVec<double>* v) const {
	return false;
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
*/
/*
template <>
inline bool LSVec<void*>::lt(const LSVec<double>* v) const {
	return false;
}

template <typename T>
inline bool LSVec<T>::lt(const LSVec<double>* v) const {
	return std::lexicographical_compare(this->begin(), this->end(), v->begin(), v->end());
}
*/
template <typename T>
bool LSVec<T>::in(LSValue* key) const {
	const LSVar* n = dynamic_cast<const LSVar*>(key);
	if (!n) return false;
	if (n->type != LSVar::REAL) return false;
	for (auto i = this->begin(); i != this->end(); i++) {
		if ((*i) == n->real) {
			return true;
		}
	}
	return false;
}

template <>
inline bool LSVec<LSValue*>::in(LSValue* key) const {
	for (auto i = this->begin(); i != this->end(); i++) {
		if (**i == *key) {
			return true;
		}
	}
	return false;
}

template <>
inline int LSVec<int>::atv(const int i) {
	return this->operator[] (i);
}

template <>
inline int* LSVec<int>::atLv(int i) {
	return &this->operator[] (i);
}

template <typename T>
inline LSValue* LSVec<T>::range(int start, int end) const {

	LSVec<T>* range = new LSVec<T>();

	size_t start_i = std::max<size_t>(0, start);
	size_t end_i = std::min<size_t>(this->size(), end);

	for (size_t i = start_i; i < end_i; ++i) {
		range->push_clone(this->operator [] (i));
	}
	return range;
}

template <class T>
LSValue* LSVec<T>::rangeL(int, int) {
	return this;
}

template <class T>
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
inline std::ostream& LSVec<LSValue*>::print(std::ostream& os) const {
	os << "[";
	for (auto i = this->begin(); i != this->end(); i++) {
		if (i != this->begin()) os << ", ";
		os << **i;
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

template <class T>
RawType LSVec<T>::getRawType() const {
	return RawType::VEC;
}

/*
template <>
inline LSValue* LSVec<LSValue*>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return ((std::vector<LSValue*>*) this)->at((int) n->value)->clone();
		} catch (std::exception& e) {
			return new LSVar();
		}
	}
	return new LSVar();
}

template <typename T>
inline LSValue* LSVec<T>::at(const LSValue* key) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(key)) {
		try {
			return new LSVar(((std::vector<T>*) this)->at((int) n->value));
		} catch (std::exception& e) {
			return new LSVar();
		}
	}
	return new LSVar();
}

template <class T>
LSValue* LSVec<T>::attr(const LSValue* key) const {

	if (*((LSString*) key) == "size") {
		return new LSVar(this->size());
	}
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return new LSVar();
}


template <class T>
LSValue** LSVec<T>::atL(const LSValue* key) {
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
*/

template <class T>
LSValue** LSVec<T>::attrL(const LSValue*) {
	return nullptr;
}

} // end of namespace ls

#endif

