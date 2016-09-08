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

	inline void reanalyse_l(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type) {
		if (analysed) reanalyse_l_help(analyser, req_type, req_left_type);
	}
	inline void finalize_l(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type) {
		if (analysed) finalize_l_help(analyser, req_type, req_left_type);
	}

	virtual jit_value_t compile_l(Compiler&) const = 0;

protected:
	void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type);
	void finalize_help(SemanticAnalyser* analyser, const Type& req_type);

	virtual void reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type) = 0;
	virtual void finalize_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type) = 0;
};

}

#endif
