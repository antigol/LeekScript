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

	static LSValue* array_class;

	LSArray();
	LSArray(std::initializer_list<T>);
	LSArray(const std::vector<T>&);
	LSArray(const LSArray<T>&);
	LSArray(Json& data);

	virtual ~LSArray();

	/*
	 * Array functions
	 */
	void push_clone(T value); // clone increment and push
	void push_move(T value); // clone (if not temporaray) increment and push
	void push_inc(T value); // increment (if not native) and push

	int int_size();

	LSArray<T>* ls_clear();
	T ls_remove(int index);
	bool ls_remove_element(T element);
	bool ls_empty();
	LSValue* ls_pop();
	int ls_size();
	T ls_sum();
	T ls_product();
	double ls_average();
	LSValue* ls_first();
	LSValue* ls_last();
	int atv(const int i);
	int* atLv(int i);
	LSArray<LSValue*>* ls_map(LSFunction* function);
	LSArray<int>* ls_map_int(LSFunction* function);
	LSArray<double>* ls_map_real(LSFunction* function);
	LSArray<LSValue*>* ls_chunk(int size = 1);
	LSValue* ls_unique();
	LSValue* ls_sort();
	void ls_iter(const void* fun);
	bool ls_contains(T val);
	LSValue* ls_push(T val);
	LSArray<T>* ls_push_all_ptr(LSArray<LSValue*>* array);
	LSArray<T>* ls_push_all_int(LSArray<int>* array);
	LSArray<T>* ls_push_all_flo(LSArray<double>* array);
	LSArray<T>* ls_shuffle();
	LSArray<T>* ls_reverse();
	LSArray<T>* ls_filter(LSFunction* fun);
	LSValue* ls_foldLeft(LSFunction* function, LSValue* initial);
	LSValue* ls_foldRight(LSFunction* function, LSValue* initial);
	LSArray<T>* ls_insert(T value, int pos);
	LSArray<LSValue*>* ls_partition(LSFunction* function);
	LSArray<LSValue*>* ls_map2(LSArray<LSValue*>*, LSFunction* function);
	LSArray<LSValue*>* ls_map2_int(LSArray<int>*, LSFunction* function);
	int ls_search(T needle, int start);
	LSString* ls_join(LSString* glue);
	LSArray<T>* ls_fill(T element, int size);
	T ls_max();
	T ls_min();
	bool is_permutation(LSArray<int>*);

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSValue* ls_not() override;
	LSValue* ls_tilde() override;

	LSVALUE_OPERATORS

	LSValue* ls_add(LSNull*) override;
	LSValue* ls_add(LSBoolean*) override;
	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString*) override;
	LSValue* ls_add(LSArray<LSValue*>*) override;
	LSValue* ls_add(LSArray<int>*) override;
	LSValue* ls_add(LSArray<double>*) override;
	LSValue* ls_add(LSObject*) override;
	LSValue* ls_add(LSFunction*) override;
	LSValue* ls_add(LSClass*) override;

	LSValue* ls_add_eq(LSNull*) override;
	LSValue* ls_add_eq(LSBoolean*) override;
	LSValue* ls_add_eq(LSNumber*) override;
	LSValue* ls_add_eq(LSString*) override;
	LSValue* ls_add_eq(LSArray<LSValue*>*) override;
	LSValue* ls_add_eq(LSArray<int>*) override;
	LSValue* ls_add_eq(LSArray<double>*) override;
	LSValue* ls_add_eq(LSSet<LSValue*>*) override;
	LSValue* ls_add_eq(LSSet<int>*) override;
	LSValue* ls_add_eq(LSSet<double>*) override;
	LSValue* ls_add_eq(LSObject*) override;
	LSValue* ls_add_eq(LSFunction*) override;
	LSValue* ls_add_eq(LSClass*) override;

	bool eq(const LSArray<LSValue*>*) const override;
	bool eq(const LSArray<int>*) const override;
	bool eq(const LSArray<double>*) const override;

	bool lt(const LSArray<LSValue*>*) const override;
	bool lt(const LSArray<int>*) const override;
	bool lt(const LSArray<double>*) const override;

	bool in(const T) const;

	LSValue* at(const LSValue* value) const override;
	LSValue** atL(const LSValue* value) override;

	LSValue* range(int start, int end) const override;
	LSValue* rangeL(int start, int end) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* clone() const override;

	LSValue* getClass() const override;

	virtual const BaseRawType* getRawType() const override;

	int typeID() const override { return 5; }
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSArray.tcc"
#endif

#endif
