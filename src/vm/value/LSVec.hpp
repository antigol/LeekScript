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
class LSVec : public LSValue, public std::vector<T> {
public:
	LSVec();
	LSVec(std::initializer_list<T>);
	LSVec(const std::vector<T>&);
	LSVec(const LSVec<T>&);
	LSVec(Json& data);

	virtual ~LSVec();

	/*
	 * Array functions
	 */
	void push_clone(T value); // clone increment and push
	void push_move(T value); // clone (if not temporaray) increment and push
	void push_inc(T value); // increment (if not native) and push

	LSVec<T>* ls_clear();
	T ls_remove(int index);
	bool ls_remove_element(T element);
	bool ls_empty();
	LSValue* ls_pop();
	int ls_size();
	T ls_sum();
	double ls_average();
	LSValue* ls_first();
	LSValue* ls_last();
	int atv(const int i);
	int* atLv(int i);
	LSVec<LSValue*>* ls_map(const void*);
	LSVec<int>* ls_map_int(const void*);
	LSVec<double>* ls_map_real(const void*);
	LSVec<LSValue*>* ls_chunk(int size = 1);
	LSValue* ls_unique();
	LSValue* ls_sort();
	void ls_iter(const void* fun);
	bool ls_contains(T val);
	LSValue* ls_push(T val);
	LSVec<T>* ls_push_all_ptr(LSVec<LSValue*>* array);
	LSVec<T>* ls_push_all_int(LSVec<int32_t>* array);
	LSVec<T>* ls_push_all_flo(LSVec<double>* array);
	LSVec<T>* ls_shuffle();
	LSVec<T>* ls_reverse();
	LSVec<T>* ls_filter(const void* fun);
	LSValue* ls_foldLeft(const void* fun, LSValue* initial);
	LSValue* ls_foldRight(const void* fun, LSValue* initial);
	LSVec<T>* ls_insert(T value, int pos);
	LSVec<LSValue*>* ls_partition(const void* fun);
	LSVec<LSValue*>* ls_map2(LSVec<LSValue*>*, const void* fun);
	LSVec<LSValue*>* ls_map2_int(LSVec<int>*, const void* fun);
	int ls_search(T needle, int start);
	LSVec<T>* ls_fill(T element, int size);
	T ls_max();
	T ls_min();

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSVALUE_OPERATORS

//	bool eq(const LSVec<LSValue*>*) const override;
//	bool eq(const LSVec<int32_t>*) const override;
//	bool eq(const LSVec<double>*) const override;

//	bool lt(const LSVec<LSValue*>*) const override;
//	bool lt(const LSVec<int32_t>*) const override;
//	bool lt(const LSVec<double>*) const override;

	bool in(LSValue*) const;

	LSValue* at(const LSValue* value) const;
	LSValue** atL(const LSValue* value);

	LSValue* range(int start, int end) const;
	LSValue* rangeL(int start, int end);

	LSValue* attr(const LSValue* key) const;
	LSValue** attrL(const LSValue* key);

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* clone() const override;

	virtual RawType getRawType() const override;

	int typeID() const override { return 5; }
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSVec.tcc"
#endif

#endif
