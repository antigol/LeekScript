#include "Continue.hpp"

using namespace std;

namespace ls {

Continue::Continue() {
	deepness = 1;
}

Continue::~Continue() {}

void Continue::print(std::ostream& os, int, bool) const {
	os << "continue";
	if (deepness > 1) {
		os << " " << deepness;
	}
}

unsigned Continue::line() const
{
	return 0;
}

// DONE 2
void Continue::analyse_help(SemanticAnalyser* analyser)
{
	// break must be in a loop
	if (!analyser->in_loop(deepness)) {
		analyser->add_error({ SemanticException::Type::CONTINUE_MUST_BE_IN_LOOP, line() });
	}
	type = Type::UNREACHABLE;
}

void Continue::reanalyse_help(SemanticAnalyser*, const Type&)
{
}

void Continue::finalize_help(SemanticAnalyser*, const Type&)
{
}

jit_value_t Continue::compile(Compiler& c) const {

	c.delete_variables_block(c.F, c.get_current_loop_blocks(deepness));

	jit_insn_branch(c.F, c.get_current_loop_cond_label(deepness));

	return nullptr;
}

}
