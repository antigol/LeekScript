#include "IndexAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSVar.hpp"
#include "../jit/jit_vec.hpp"

using namespace std;

namespace ls {

IndexAccess::IndexAccess() {
	container = nullptr;
	key = nullptr;
	key2 = nullptr;
}

IndexAccess::~IndexAccess() {
	delete container;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void IndexAccess::print(std::ostream& os, int indent, bool debug) const {
	container->print(os, indent, debug);
	os << "[";
	key->print(os, indent, debug);
	if (key2) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

unsigned IndexAccess::line() const {
	return 0;
}

bool IndexAccess::isLeftValue() const
{
	return key2 == nullptr && container->isLeftValue();
}

// DONE 2
void IndexAccess::analyse_help(SemanticAnalyser* analyser)
{
	container->analyse(analyser);

	key->analyse(analyser);
	if (key2) {
		key2->analyse(analyser);

		if (container->type.get_raw_type() == &RawType::VEC) {
			type = Type(&RawType::VEC, { container->type.element_type(0) });
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			type = Type(&RawType::VEC, { container->type.element_type(1) });
		} else {
			type = Type::VEC;
		}

	} else {
		if (container->type.get_raw_type() == &RawType::VEC) {
			left_type = container->type.element_type(0);
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			left_type = container->type.element_type(1);
		} else {
			left_type = Type::UNKNOWN;
		}

		type = left_type.image_conversion();
	}
}

void IndexAccess::reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (!Type::intersection(left_type, req_left_type, &left_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (key2) {

		Type fiber = type.element_type(0).fiber_conversion();
		Type container_req_type = Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) });

		container->reanalyse(analyser, container_req_type);

		Type element_type;

		if (container->type.get_raw_type() == &RawType::VEC) {
			element_type = container->type.element_type(0);
			key->reanalyse(analyser, Type::I32);
			key2->reanalyse(analyser, Type::I32);
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			element_type = container->type.element_type(1);
			Type key_type = container->type.element_type(0);
			key->reanalyse(analyser, key_type);
			key2->reanalyse(analyser, key_type);
		} else {
			element_type = Type::UNKNOWN;
			key->reanalyse(analyser, Type::UNKNOWN);
			key2->reanalyse(analyser, Type::UNKNOWN);
		}

		if (!Type::intersection(type, Type(&RawType::VEC, { element_type.image_conversion() }), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

	} else {
		Type fiber = type.fiber_conversion();
		Type container_req_type = Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) });

		Type element_type;

		if (isLeftValue()) {
			LeftValue* left_container = dynamic_cast<LeftValue*>(container);

			Type container_req_left_type = Type({ Type(&RawType::VEC, { left_type }), Type(&RawType::MAP, { Type::UNKNOWN, left_type }) });

			left_container->reanalyse_l(analyser, container_req_type, container_req_left_type);

			Type element_left_type;

			if (left_container->left_type.get_raw_type() == &RawType::VEC) {
				element_left_type = left_container->left_type.element_type(0);
				element_type = left_container->type.element_type(0);
				key->reanalyse(analyser, Type::I32);
			} else if (left_container->left_type.get_raw_type() == &RawType::MAP) {
				element_left_type = left_container->left_type.element_type(1);
				element_type = left_container->type.element_type(1);
				key->reanalyse(analyser, left_container->left_type.element_type(0));
			} else {
				element_left_type = Type::UNKNOWN;
				element_type = Type::UNKNOWN;
				key->reanalyse(analyser, Type::UNKNOWN);
			}

			if (!Type::intersection(left_type, element_left_type, &left_type)) {
				add_error(analyser, SemanticException::TYPE_MISMATCH);
			}

		} else {
			container->reanalyse(analyser, container_req_type);

			if (container->type.get_raw_type() == &RawType::VEC) {
				element_type = container->type.element_type(0);
				key->reanalyse(analyser, Type::I32);
			} else if (container->type.get_raw_type() == &RawType::MAP) {
				element_type = container->type.element_type(1);
				Type key_type = container->type.element_type(0);
				key->reanalyse(analyser, key_type);
			} else {
				element_type = Type::UNKNOWN;
				key->reanalyse(analyser, Type::UNKNOWN);
			}
		}

		if (!Type::intersection(type, element_type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}

	}
}

void IndexAccess::finalize_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (!Type::intersection(left_type, req_left_type, &left_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	if (key2) {

		Type fiber = type.element_type(0).fiber_conversion();
		Type container_req_type = Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) });

		Type element_type;

		container->finalize(analyser, container_req_type);

		if (container->type.get_raw_type() == &RawType::VEC) {
			element_type = container->type.element_type(0);
			key->finalize(analyser, Type::I32);
			key2->finalize(analyser, Type::I32);
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			element_type = container->type.element_type(1);
			Type key_type = container->type.element_type(0);
			key->finalize(analyser, key_type);
			key2->finalize(analyser, key_type);
		} else {
			assert(0);
		}


		if (!Type::intersection(type, Type(&RawType::VEC, { element_type.image_conversion() }), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();

	} else {

		Type fiber = type.fiber_conversion();
		Type container_req_type = Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) });

		Type element_type;

		if (isLeftValue()) {
			LeftValue* left_container = dynamic_cast<LeftValue*>(container);

			Type container_req_left_type = Type({ Type(&RawType::VEC, { left_type }), Type(&RawType::MAP, { Type::UNKNOWN, left_type }) });

			left_container->finalize_l(analyser, container_req_type, container_req_left_type);

			if (left_container->left_type.get_raw_type() == &RawType::VEC) {
				left_type = left_container->left_type.element_type(0);
				element_type = left_container->type.element_type(0);
				key->finalize(analyser, Type::I32);
			} else if (left_container->left_type.get_raw_type() == &RawType::MAP) {
				left_type = left_container->left_type.element_type(1);
				element_type = left_container->type.element_type(1);
				key->finalize(analyser, left_container->left_type.element_type(0));
			} else {
				assert(0);
			}
			assert(left_type.is_pure() || !analyser->errors.empty());

		} else {
			container->finalize(analyser, container_req_type);

			if (container->type.get_raw_type() == &RawType::VEC) {
				element_type = container->type.element_type(0);
				key->finalize(analyser, Type::I32);
			} else if (container->type.get_raw_type() == &RawType::MAP) {
				element_type = container->type.element_type(1);
				Type key_type = container->type.element_type(0);
				key->finalize(analyser, key_type);
			} else {
				assert(0);
			}
		}

		if (!Type::intersection(type, element_type.image_conversion(), &type)) {
			add_error(analyser, SemanticException::TYPE_MISMATCH);
		}
		type.make_it_pure();
	}

	assert(type.is_pure() || !analyser->errors.empty());
}

jit_value_t IndexAccess::compile(Compiler& c) const {
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	if (!key2) {
		jit_value_t val = nullptr;

		if (container->type.raw_type == &RawType::VEC) val = jit_vec::index_delete_temporary(c.F, container->type.elements_types[0], a, k);

		assert(val);
		return jit_general::convert(c.F, val, container->type.element_type(0), type);

	} else {
		jit_value_t k2 = key2->compile(c);
		jit_value_t out = jit_vec::create(c.F);

		if (container->type.raw_type == &RawType::VEC) {
			jit_type_t jit_element_type = container->type.elements_types[0].jit_type();

			pair<jit_value_t, jit_value_t> begin_end = jit_vec::begin_end(c.F, a);

			jit_value_t it = begin_end.first;
			jit_value_t end = begin_end.second;

			jit_value_t it1 = jit_insn_load_elem_address(c.F, it, k, jit_element_type);
			jit_value_t it2 = jit_insn_load_elem_address(c.F, it, k2, jit_element_type);

			jit_label_t stop = jit_label_undefined;
			jit_label_t loop = jit_label_undefined;

			// x[-4:5]
			jit_insn_branch_if(c.F, jit_insn_lt(c.F, it1, it), &loop);
			jit_insn_store(c.F, it, it1);

			jit_insn_label(c.F, &loop);
			jit_insn_branch_if(c.F, jit_insn_ge(c.F, it, end), &stop);
			jit_insn_branch_if(c.F, jit_insn_ge(c.F, it, it2), &stop);

			jit_value_t conv = jit_general::convert(c.F, jit_insn_load_relative(c.F, it, 0, jit_element_type), container->type.elements_types[0], type.elements_types[0]);
			jit_vec::push_move_inc(c.F, type.elements_types[0], out, conv);

			jit_insn_store(c.F, it, jit_insn_add_relative(c.F, it, jit_type_get_size(jit_element_type)));

			jit_insn_branch(c.F, &loop);
			jit_insn_label(c.F, &stop);

			jit_type_free(jit_element_type);
		} else {
			assert(0);
		}

		jit_general::delete_temporary(c.F, a, container->type);

		return out;
	}

	assert(0);
	return nullptr;
}

jit_value_t IndexAccess::compile_l(Compiler& c) const
{
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	if (container->type.raw_type == &RawType::VEC) return jit_vec::index_l(c.F, container->type.elements_types[0], a, k);

	assert(0);
	return nullptr;
}

}
