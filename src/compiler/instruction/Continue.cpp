#include "../../compiler/instruction/Continue.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

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

void Continue::analyse(SemanticAnalyser* analyser, const Type&) {

	// continue must be in a loop
	if (!analyser->in_loop(deepness)) {
		analyser->add_error({SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, 0});
	}
}

jit_value_t Continue::compile(Compiler& c) const {

	c.delete_variables_block(c.F, c.get_current_loop_blocks(deepness));

	jit_insn_branch(c.F, c.get_current_loop_cond_label(deepness));

	return nullptr;
}

}
