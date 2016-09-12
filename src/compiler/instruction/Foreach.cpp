#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../jit/jit_vec.hpp"

using namespace std;

namespace ls {

Foreach::Foreach() {
	key = nullptr;
	value = nullptr;
	body = nullptr;
	container = nullptr;
	key_var = nullptr;
	value_var = nullptr;
}

Foreach::~Foreach() {
	delete container;
	delete body;
}

void Foreach::print(ostream& os, int indent, bool debug) const {
	os << "for ";

	if (key != nullptr) {
		os << key->content;
		if (debug) {
			os << " " << key_var->type;
		}
		os << " : ";
	}
	os << value->content;
	if (debug) {
		os << " " << value_var->type;
	}

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
}

unsigned Foreach::line() const
{
	return 0;
}

// DONE 2
void Foreach::analyse_help(SemanticAnalyser* analyser)
{
	if (type.raw_type != &RawType::VEC) {
		type = Type::VOID;
	}

	analyser->enter_block(this);

	container->analyse(analyser);

	if (key != nullptr) {
		key_var = analyser->add_var(key, Type::UNKNOWN, this, nullptr);
	}
	value_var = analyser->add_var(value, Type::UNKNOWN, this, nullptr);

	analyser->enter_loop();

	body->analyse(analyser);

	analyser->leave_loop();
	analyser->leave_block();
}

void Foreach::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	container->reanalyse(analyser, Type({ Type::VEC, Type::MAP, Type::SET }));

	if (container->type.get_raw_type() == &RawType::VEC || container->type.get_raw_type() == &RawType::SET) {
		if (key_var && !Type::intersection(key_var->type, Type::I32, &key_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		if (!Type::intersection(value_var->type, container->type.element_type(0), &value_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else if (container->type.get_raw_type() == &RawType::MAP) {
		if (key_var && !Type::intersection(key_var->type, container->type.element_type(0), &key_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		if (!Type::intersection(value_var->type, container->type.element_type(1), &value_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}

	if (type.raw_type == &RawType::VEC) {
		body->reanalyse(analyser, type.element_type(0));

		if (!Type::intersection(type.elements_types[0], body->type, &type.elements_types[0])) {
			body->add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		if (body->type == Type::VOID || body->type == Type::UNREACHABLE) {
			body->add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else {
		body->reanalyse(analyser, Type::VOID);
	}
}

void Foreach::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	container->finalize(analyser, Type({ Type::VEC, Type::MAP, Type::SET }));

	if (container->type.get_raw_type() == &RawType::VEC || container->type.get_raw_type() == &RawType::SET) {
		if (key_var && !Type::intersection(key_var->type, Type::I32, &key_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		if (!Type::intersection(value_var->type, container->type.element_type(0), &value_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	} else if (container->type.get_raw_type() == &RawType::MAP) {
		if (key_var && !Type::intersection(key_var->type, container->type.element_type(0), &key_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		if (!Type::intersection(value_var->type, container->type.element_type(1), &value_var->type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
	}

	if (key_var) key_var->type.make_it_pure();
	value_var->type.make_it_pure();

	if (type.raw_type == &RawType::VEC) {
		body->finalize(analyser, type.element_type(0));
		type.set_element_type(0, body->type);
	} else {
		body->finalize(analyser, Type::VOID);
	}
}

jit_value_t Foreach::compile(Compiler& c) const
{
	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	jit_value_t output_v = nullptr;
	if (type.raw_type == &RawType::VEC) {
		output_v = jit_vec::create(c.F);
		jit_vec::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	// Container (Array, Map or Set)
	jit_value_t container_v = container->compile(c);

	jit_general::inc_refs(c.F, container_v, container->type);

	c.add_var("{array}", container_v, container->type, false);

	// Create variables
	jit_type_t jit_value_type = value_var->type.jit_type();
	jit_value_t value_v = jit_value_create(c.F, jit_value_type);
	jit_type_t jit_key_type = key ? key_var->type.jit_type() : nullptr;
	jit_value_t key_v = key ? jit_value_create(c.F, jit_key_type) : nullptr;

	c.add_var(value->content, value_v, value_var->type, true);
	if (key) c.add_var(key->content, key_v, key_var->type, true);


	jit_label_t label_end = jit_label_undefined;
	jit_label_t label_it = jit_label_undefined;
	c.enter_loop(&label_end, &label_it);

	if (container->type.raw_type == &RawType::VEC) {
		// iterators
		pair<jit_value_t, jit_value_t> begin_end = jit_vec::begin_end(c.F, container_v);
		jit_value_t begin = begin_end.first;
		jit_value_t end = begin_end.second;

		if (key != nullptr) {
			jit_insn_store(c.F, key_v, jit_general::constant_i32(c.F, 0));
		}

		// cond label:
		jit_label_t label_cond = jit_label_undefined;
		jit_insn_label(c.F, &label_cond);

		// Condition to continue
		jit_insn_branch_if(c.F, jit_insn_eq(c.F, begin, end), &label_end);

		// Get Value
		jit_insn_store(c.F, value_v, jit_insn_load_relative(c.F, begin, 0, jit_value_type));

		// Body
		jit_value_t body_v = body->compile(c);
		if (output_v && body_v) {
			jit_vec::push_move_inc(c.F, type.elements_types[0], output_v, body_v);
		}


		// it++
		jit_insn_label(c.F, &label_it);

		// incrment key
		if (key != nullptr) {
			jit_insn_store(c.F, key_v, jit_insn_add(c.F, key_v, jit_general::constant_i32(c.F, 1)));
		}

		jit_insn_store(c.F, begin, jit_insn_add_relative(c.F, begin, jit_type_get_size(jit_value_type)));

		// jump to cond
		jit_insn_branch(c.F, &label_cond);
	} else {
		assert(0);
	}
	c.leave_loop();

	// end label:
	jit_insn_label(c.F, &label_end);

	jit_value_t return_v = jit_general::move(c.F, output_v, type); // otherwise it is delete by the c.leave_block
	c.leave_block(c.F); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block

	return return_v;
}

}
