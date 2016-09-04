#ifndef LEFTVALUE_HPP_
#define LEFTVALUE_HPP_

#include "Value.hpp"

namespace ls {

class LeftValue : public Value {
public:

	LeftValue();
	virtual ~LeftValue();

	virtual bool isLeftValue() const override;

	virtual jit_value_t compile_l(Compiler&) const = 0;
};

}

#endif
