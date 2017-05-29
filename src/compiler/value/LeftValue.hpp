#ifndef LEFTVALUE_HPP_
#define LEFTVALUE_HPP_

#include "Value.hpp"
#include "../Compiler.hpp"

namespace ls {

class LeftValue : public Value {
public:

	LeftValue() = default;
	virtual ~LeftValue() = default;

	virtual bool isLeftValue() const override;
	virtual void change_type(SemanticAnalyser*, const Type&) = 0;

	virtual Compiler::value compile_l(Compiler&) const = 0;
};

}

#endif
