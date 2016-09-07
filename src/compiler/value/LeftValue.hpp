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

	inline void reanalyse_l(SemanticAnalyser* analyser, const Type& req_type) {
		if (analysed) reanalyse_l_help(analyser, req_type);
	}

	virtual jit_value_t compile_l(Compiler&) const = 0;

protected:
	virtual void reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type) = 0;
};

}

#endif
