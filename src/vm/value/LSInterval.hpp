/*
 * LSInterval.hpp
 *
 *  Created on: 5 mai 2016
 *      Author: pierre
 */

#ifndef VM_VALUE_LSINTERVAL_HPP_
#define VM_VALUE_LSINTERVAL_HPP_

#include "LSArray.hpp"

namespace ls {

class LSInterval : public LSArray<int> {
public:

	int a = 0;
	int b = 0;

	LSInterval();
	virtual ~LSInterval();

	/*
	 * Array methods
	 */
	LSArray<int>* ls_filter(const void* function);

	/*
	 * LSValue methods
	 */
	bool in_v(int) const;

	int atv(const int key) const;
	virtual LSValue* at(const LSValue* key) const override;
	virtual LSValue** atL(const LSValue* key) override;
	virtual LSValue* abso() const override;

	virtual LSValue* clone() const override;
	std::ostream& print(std::ostream& os) const override;
};

}

#endif
