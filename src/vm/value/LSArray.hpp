/*
 * Describe an array in LeekScript
 */
#ifndef LS_ARRAY_BASE
#define LS_ARRAY_BASE

#include <vector>
#include <iterator>

#include "../LSValue.hpp"

namespace ls {

template <typename T>
class LSArray : public LSValue, public std::vector<T> {
public:

	static LSValue* clazz;

	LSArray();
	LSArray(std::initializer_list<T>);
	LSArray(const std::vector<T>&);
	LSArray(const LSArray<T>&);

	virtual ~LSArray();

	/*
	 * Array functions
	 */
	void push_clone(T value); // clone increment and push
	void push_move(T value); // clone (if not temporary) increment and push
	void push_inc(T value); // increment (if not native) and push
	int int_size();
	LSArray<T>* ls_clear();
	T ls_remove(int index);
	bool ls_remove_element(T element);
	bool ls_empty();
	T ls_pop();
	int ls_size();
	T ls_sum();
	T ls_product();
	double ls_average();
	T ls_first();
	T ls_last();
	template <class R>
	LSArray<R>* ls_map(LSFunction<R>* function);
	//LSArray<T>* ls_map(LSFunction<T>* function);
	LSArray<LSValue*>* ls_chunk(int size = 1);
	LSArray<T>* ls_unique();
	LSArray<T>* ls_sort();
	void ls_iter(LSFunction<LSValue*>* fun);
	bool ls_contains(T val);
	LSArray<T>* ls_push(T val);
	LSArray<T>* ls_push_all_ptr(LSArray<LSValue*>* array);
	LSArray<T>* ls_push_all_int(LSArray<int>* array);
	LSArray<T>* ls_push_all_flo(LSArray<double>* array);
	LSArray<T>* ls_shuffle();
	LSArray<T>* ls_reverse();
	LSArray<T>* ls_filter(LSFunction<bool>* fun);
	template <class R>
	R ls_foldLeft(LSFunction<R>* function, R initial);
	template <class R>
	R ls_foldRight(LSFunction<R>* function, R initial);
	LSArray<T>* ls_insert(T value, int pos);
	LSArray<LSValue*>* ls_partition(LSFunction<bool>* function);
	template <class R, class T2>
	LSArray<R>* ls_map2(LSArray<T2>*, LSFunction<R>* function);
	int ls_search(T needle, int start);
	LSString* ls_join(LSString* glue);
	LSArray<T>* ls_fill(T element, int size);
	T ls_max();
	T ls_min();
	bool is_permutation(LSArray<int>*);
	template <class T2>
	LSValue* add_set(LSSet<T2>* set);
	LSArray<T>* ls_random(int n);

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	LSValue* ls_tilde() override;

	LSValue* add(LSValue* v) override;
	LSValue* add_eq(LSValue* v) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	virtual bool in(const LSValue* const) const override;
	virtual bool in_i(const int) const override;

	LSValue* at(const LSValue* value) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* range(int start, int end) const override;

	int abso() const override;

	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;
	LSValue* clone() const override;
	LSValue* getClass() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSArray.tcc"
#endif

#endif
