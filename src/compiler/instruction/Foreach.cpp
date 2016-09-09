#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSSet.hpp"

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

/*
 * Begin
 */
LSVec<LSValue*>::iterator fun_begin_vec_all(LSVec<LSValue*>* array) {
	return array->begin();
}
LSMap<LSValue*,LSValue*>::iterator fun_begin_map_all(LSMap<LSValue*,LSValue*>* map) {
	return map->begin();
}
LSSet<LSValue*>::iterator fun_begin_set_all(LSSet<LSValue*>* set) {
	return set->begin();
}

/*
 * Condition
 */
bool fun_condition_vec_all(LSVec<LSValue*>* array, LSVec<LSValue*>::iterator it) {
	return it != array->end();
}
bool fun_condition_map_all(LSMap<LSValue*,LSValue*>* map, LSMap<LSValue*,LSValue*>::iterator it) {
	return it != map->end();
}
bool fun_condition_set_all(LSSet<LSValue*>* set, LSSet<LSValue*>::iterator it) {
	return it != set->end();
}

/*
 * Value
 */
template <typename T>
T fun_value_vec(typename LSVec<T>::iterator it) {
	return *it;
}
template <typename K, typename T>
T fun_value_map(typename LSMap<K,T>::iterator it) {
	return it->second;
}
template <typename T>
T fun_value_set(typename LSSet<T>::iterator it) {
	return *it;
}


/*
 * Key
 */
template <typename T>
int fun_key_vec(LSVec<T>* vec, typename LSVec<T>::iterator it) {
	return distance(vec->begin(), it);
}
template <typename K, typename T>
K fun_key_map(LSMap<K,T>*, typename LSMap<K,T>::iterator it) {
	return it->first;
}
template <typename T>
int fun_key_set(LSSet<T>* set, typename LSSet<T>::iterator it) {
	return distance(set->begin(), it);
}



/*
 * Increment
 */
template <typename T>
typename LSVec<T>::iterator fun_inc_vec(typename LSVec<T>::iterator it) {
	return ++it;
}
template <typename K, typename T>
typename LSMap<K,T>::iterator fun_inc_map(typename LSMap<K,T>::iterator it) {
	return ++it;
}
template <typename T>
typename LSSet<T>::iterator fun_inc_set(typename LSSet<T>::iterator it) {
	return ++it;
}

jit_value_t Foreach::compile(Compiler& c) const {

	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	jit_value_t output_v = nullptr;
	if (type.raw_type == &RawType::VEC) {
		output_v = VM::create_vec(c.F, type.element_type(0));
		VM::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	// Container (Array, Map or Set)
	jit_value_t container_v = container->compile(c);

	if (container->type.must_manage_memory()) {
		VM::inc_refs(c.F, container_v);
	}

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

	// Static Selector
	if (container->type == Type(&RawType::VEC, { Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_vec_all, (void*) fun_condition_vec_all, (void*) fun_value_vec<int32_t>, (void*) fun_key_vec<int32_t>, (void*) fun_inc_vec<int32_t>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::VEC, { Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_vec_all, (void*) fun_condition_vec_all, (void*) fun_value_vec<double>, (void*) fun_key_vec<double>, (void*) fun_inc_vec<double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::VEC, { Type::LSVALUE }))) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_vec_all, (void*) fun_condition_vec_all, (void*) fun_value_vec<LSValue*>, (void*) fun_key_vec<LSValue*>, (void*) fun_inc_vec<LSValue*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::VEC, { Type::UNKNOWN }))) { // tout le reste == functions
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_vec_all, (void*) fun_condition_vec_all, (void*) fun_value_vec<void*>, (void*) fun_key_vec<void*>, (void*) fun_inc_vec<void*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::MAP, { Type::LSVALUE, Type::LSVALUE }))) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<LSValue*,LSValue*>, (void*) fun_key_map<LSValue*,LSValue*>, (void*) fun_inc_map<LSValue*,LSValue*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::MAP, { Type::LSVALUE, Type::I32 }))) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<LSValue*,int32_t>, (void*) fun_key_map<LSValue*,int32_t>, (void*) fun_inc_map<LSValue*,int32_t>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::MAP, { Type::LSVALUE, Type::F64 }))) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<LSValue*,double>, (void*) fun_key_map<LSValue*,double>, (void*) fun_inc_map<LSValue*,double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (Type::intersection(container->type, Type(&RawType::MAP, { Type::I32, Type::LSVALUE }))) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<int32_t,LSValue*>, (void*) fun_key_map<int32_t,LSValue*>, (void*) fun_inc_map<int32_t,LSValue*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::MAP, { Type::I32, Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<int32_t,int32_t>, (void*) fun_key_map<int32_t,int32_t>, (void*) fun_inc_map<int32_t,int32_t>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::MAP, { Type::I32, Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map<int32_t,double>, (void*) fun_key_map<int32_t,double>, (void*) fun_inc_map<int32_t,double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::SET, { Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<int>, (void*) fun_key_set<int>, (void*) fun_inc_set<int>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::SET, { Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<double>, (void*) fun_key_set<double>, (void*) fun_inc_set<double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(&RawType::SET, { Type::VAR })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<LSValue*>, (void*) fun_key_set<LSValue*>, (void*) fun_inc_set<LSValue*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	}

	c.leave_loop();

	// end label:
	jit_insn_label(c.F, &label_end);

	jit_value_t return_v = VM::clone_obj(c.F, output_v); // otherwise it is delete by the c.leave_block
	c.leave_block(c.F); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block

	return return_v;
}

void Foreach::compile_foreach(Compiler&c, jit_value_t container_v, jit_value_t output_v,
							  void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
							  jit_label_t* label_it, jit_label_t* label_end,
							  jit_type_t jit_value_type, jit_value_t value_v, jit_type_t jit_key_type, jit_value_t key_v) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;

	// Variable it = begin()

	jit_value_t it_v = jit_value_create(c.F, LS_POINTER);
	jit_insn_store(c.F, it_v, Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) fun_begin, { container_v }));


	// cond label:
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	jit_value_t cond_v = Compiler::call_native(c.F, jit_type_sys_bool, { LS_POINTER, LS_POINTER }, (void*) fun_condition, { container_v, it_v });
	jit_insn_branch_if_not(c.F, cond_v, label_end);

	// Get Value
	jit_insn_store(c.F, value_v, Compiler::call_native(c.F, jit_value_type, { LS_POINTER }, (void*) fun_value, { it_v }));

	// Get Key
	if (key != nullptr) {
		jit_insn_store(c.F, key_v, Compiler::call_native(c.F, jit_key_type, { LS_POINTER, LS_POINTER }, (void*) fun_key, { container_v, it_v }));
	}

	// Body
	jit_value_t body_v = body->compile(c);
	if (output_v && body_v) {
		VM::push_move_inc_vec(c.F, type.element_type(0), output_v, body_v);
	}


	// it++
	jit_insn_label(c.F, label_it);

	jit_insn_store(c.F, it_v, Compiler::call_native(c.F, LS_POINTER, { LS_POINTER }, (void*) fun_inc, { it_v }));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);
}


}
