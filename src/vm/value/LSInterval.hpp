#ifndef VM_VALUE_LSINTERVAL_HPP_
#define VM_VALUE_LSINTERVAL_HPP_

#include "../LSValue.hpp"

namespace ls {

template <class R> class LSFunction;

class LSInterval : public LSValue {
public:

	static LSValue* clazz;

	int a = 0;
	int b = 0;

	LSInterval();
	virtual ~LSInterval();

	/*
	 * Array methods
	 */
	LSArray<int>* ls_filter(LSFunction<bool>* function);
	long ls_sum();
	long ls_product();

	/*
	 * LSValue methods
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;
	virtual bool eq(const LSValue*) const override;
	virtual bool in(const LSValue* const) const override;
	virtual bool in_i(const int) const override;

	int atv(const int key) const;
	virtual LSValue* at(const LSValue* key) const override;
	LSValue* range(int start, int end) const override;
	virtual int abso() const override;

	virtual LSValue* clone() const override;
	virtual std::ostream& dump(std::ostream& os) const override;
	LSValue* getClass() const override;
};

}

#endif
