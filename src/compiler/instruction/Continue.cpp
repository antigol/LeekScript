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

Location Continue::location() const {
	return {{0, 0, 0}, {0, 0, 0}};
}

void Continue::analyse(SemanticAnalyser* analyser, const Type&) {

	// continue must be in a loop
	if (!analyser->in_loop(deepness)) {
		analyser->add_error({SemanticError::Type::CONTINUE_MUST_BE_IN_LOOP, {{0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}}}); // TODO location
	}
}

Compiler::value Continue::compile(Compiler& c) const {

	c.delete_variables_block(c.get_current_loop_blocks(deepness));

	jit_insn_branch(c.F, c.get_current_loop_cond_label(deepness));

	return {nullptr, Type::UNKNOWN};
}

Instruction* Continue::clone() const {
	auto c = new Continue();
	c->deepness = deepness;
	return c;
}

}
