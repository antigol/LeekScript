#include "../../compiler/instruction/Break.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticException.hpp"

using namespace std;

namespace ls {

Break::Break() {
	deepness = 1;
}

Break::~Break() {}

void Break::print(ostream& os, int, bool) const {
	os << "break";
	if (deepness > 1) {
		os << " " << deepness;
	}
}

unsigned Break::line() const
{
	return 0;
}

void Break::analyse_help(SemanticAnalyser* analyser)
{
	// break must be in a loop
	if (!analyser->in_loop(deepness)) {
		analyser->add_error({ SemanticException::Type::BREAK_MUST_BE_IN_LOOP, line() });
	}
	type = Type::UNREACHABLE;
}

void Break::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
}

void Break::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	analyse(analyser);
}

jit_value_t Break::compile(Compiler& c) const {

	/*	{ for {
	 *		let x = ...
	 *		{
	 *			let y = ...
	 *			if ... break => delete y, delete x, goto end
	 *			let z = ...
	 *		}
	 *		let w = ...
	 *	}
	 *		label end
	 *	}
	 */

	c.delete_variables_block(c.F, c.get_current_loop_blocks(deepness));

	jit_insn_branch(c.F, c.get_current_loop_end_label(deepness));

	return nullptr;
}

}
