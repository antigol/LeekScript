#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>

#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

class Value {
public:

	Type type;
	bool constant;
	bool parenthesis = false;

#if DEBUG >= 1
	static int reanalyse_deepness;
	static int reanalyse_maximum_deepness;
#endif

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const = 0;

	virtual unsigned line() const = 0;


	inline void analyse(SemanticAnalyser* analyser) {
		assert(analyser->in_phase == 1);
		analyse_help(analyser);
		analysed = true;
	}

	inline void reanalyse(SemanticAnalyser* analyser, const Type& req_type) {
#if DEBUG >= 1
		assert(analyser->in_phase <= 2);
		reanalyse_deepness++;
		if (reanalyse_deepness > reanalyse_maximum_deepness) reanalyse_maximum_deepness = reanalyse_deepness;
#endif

		if (analysed) reanalyse_help(analyser, req_type);

#if DEBUG >= 1
		reanalyse_deepness--;
#endif
	}

	void finalize(SemanticAnalyser* analyser, const Type& req_type) {
#if DEBUG >= 1
		assert(analyser->in_phase == 3);
#endif

		finalize_help(analyser, req_type);
		analysed = false;
	}

	virtual jit_value_t compile(Compiler&) const = 0;

	void add_error(SemanticAnalyser* analyser, SemanticException::Type error_type);
	static std::string tabs(int indent);

protected:
	/* Obligations
	 *  - init type
	 *  - call analyser methods (add_var enter_block...)
	 */
	virtual void analyse_help(SemanticAnalyser* analyser) = 0;

	/* Obligations
	 *  - do NOT call analyser methods (add_var enter_block...)
	 *  - new type must blong in the intersection of old type and req_type
	 */
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) = 0;

	/* Obligations
	 *  - do NOT call analyser methods (add_var enter_block...)
	 *  - new type must blong in the intersection of old type and req_type
	 *  - new type must be pure
	 */
	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) = 0;

	bool analysed;
};

}

#endif
