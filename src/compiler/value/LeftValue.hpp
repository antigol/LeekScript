#ifndef LEFTVALUE_HPP_
#define LEFTVALUE_HPP_

#include "Value.hpp"

namespace ls {

class LeftValue : public Value {
public:
	Type left_type;

	LeftValue();
	virtual ~LeftValue();

	virtual bool isLeftValue() const override;

	// When call `will_take` ?
	//  During preanalyse, call only on leftvalue part
	//  x = y         called on x
	//  z = x + y     called on z
	virtual void will_take(SemanticAnalyser* analyser, const Type& req_type) = 0;


	virtual jit_value_t compile_l(Compiler&) const = 0;
};

}

#endif
