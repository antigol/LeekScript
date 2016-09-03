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
		os << " : ";
	}
	os << value->content;

	os << " in ";
	container->print(os, indent + 1, debug);

	os << " ";
	body->print(os, indent, debug);
}

void Foreach::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	if (req_type.raw_type == RawType::VEC) {
		type = req_type;
	} else {
		type = Type::VOID;
	}

	analyser->enter_block();

	container->analyse(analyser, Type::UNKNOWN);

	if (container->type.raw_type == RawType::VEC || container->type.raw_type == RawType::SET) {
		key_type = Type::I32; // If no key type in array key = 0, 1, 2...
		value_type = container->type.element_type(0);
	} else if (container->type.raw_type == RawType::MAP) {
		key_type = container->type.element_type(0);
		value_type = container->type.element_type(1);
	} else {
		analyser->add_error({ SemanticException::TYPE_MISMATCH });
	}

	if (key != nullptr) {
		key_var = analyser->add_var(key, key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value, value_type, nullptr, nullptr);


	analyser->enter_loop();
	if (type == Type::VOID) {
		body->analyse(analyser, Type::VOID);
	} else {
		body->analyse(analyser, type.element_type(0));
		if (type.element_type(0) == Type::UNKNOWN) {
			type.set_element_type(0, body->type);
		} else if (type.element_type(0) != body->type) {
			analyser->add_error({ SemanticException::TYPE_MISMATCH });
		}
	}
	analyser->leave_loop();
	analyser->leave_block();
	assert(type.is_complete());
}

/*
 * Begin
 */
LSVec<LSValue*>::iterator fun_begin_array_all(LSVec<LSValue*>* array) {
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
bool fun_condition_array_all(LSVec<LSValue*>* array, LSVec<LSValue*>::iterator it) {
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
LSValue* fun_value_array_ptr(LSVec<LSValue*>::iterator it) {
	return *it;
}
int fun_value_array_int(LSVec<int>::iterator it) {
	return *it;
}
double fun_value_array_float(LSVec<double>::iterator it) {
	return *it;
}
LSValue* fun_value_map_ptr_ptr(LSMap<LSValue*,LSValue*>::iterator it) {
	return it->second;
}
int fun_value_map_ptr_int(LSMap<LSValue*,int>::iterator it) {
	return it->second;
}
double fun_value_map_ptr_float(LSMap<LSValue*,double>::iterator it) {
	return it->second;
}
LSValue* fun_value_map_int_ptr(LSMap<int,LSValue*>::iterator it) {
	return it->second;
}
int fun_value_map_int_int(LSMap<int,int>::iterator it) {
	return it->second;
}
double fun_value_map_int_float(LSMap<int,double>::iterator it) {
	return it->second;
}
template <typename T>
T fun_value_set(typename LSSet<T>::iterator it) {
	return *it;
}


/*
 * Key
 */
int fun_key_array_ptr(LSVec<LSValue*>* array, LSVec<LSValue*>::iterator it) {
	return distance(array->begin(), it);
}
int fun_key_array_int(LSVec<int>* array, LSVec<int>::iterator it) {
	return distance(array->begin(), it);
}
int fun_key_array_float(LSVec<double>* array, LSVec<double>::iterator it) {
	return distance(array->begin(), it);
}
LSValue* fun_key_map_ptr_ptr(void*, LSMap<LSValue*,LSValue*>::iterator it) {
	return it->first;
}
LSValue* fun_key_map_ptr_int(void*, LSMap<LSValue*,int>::iterator it) {
	return it->first;
}
LSValue* fun_key_map_ptr_float(void*, LSMap<LSValue*,double>::iterator it) {
	return it->first;
}
int fun_key_map_int_ptr(void*, LSMap<int,LSValue*>::iterator it) {
	return it->first;
}
int fun_key_map_int_int(void*, LSMap<int,int>::iterator it) {
	return it->first;
}
int fun_key_map_int_float(void*, LSMap<int,double>::iterator it) {
	return it->first;
}
template <typename T>
int fun_key_set(LSSet<T>* set, typename LSSet<T>::iterator it) {
	return distance(set->begin(), it);
}



/*
 * Increment
 */
LSVec<LSValue*>::iterator fun_inc_array_ptr(LSVec<LSValue*>::iterator it) {
	return ++it;
}
LSVec<int>::iterator fun_inc_array_int(LSVec<int>::iterator it) {
	return ++it;
}
LSVec<double>::iterator fun_inc_array_float(LSVec<double>::iterator it) {
	return ++it;
}
LSMap<LSValue*,LSValue*>::iterator fun_inc_map_ptr_ptr(LSMap<LSValue*,LSValue*>::iterator it) {
	return ++it;
}
LSMap<LSValue*,int>::iterator fun_inc_map_ptr_int(LSMap<LSValue*,int>::iterator it) {
	return ++it;
}
LSMap<LSValue*,double>::iterator fun_inc_map_ptr_float(LSMap<LSValue*,double>::iterator it) {
	return ++it;
}
LSMap<int,LSValue*>::iterator fun_inc_map_int_ptr(LSMap<int,LSValue*>::iterator it) {
	return ++it;
}
LSMap<int,int>::iterator fun_inc_map_int_int(LSMap<int,int>::iterator it) {
	return ++it;
}
LSMap<int,double>::iterator fun_inc_map_int_float(LSMap<int,double>::iterator it) {
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
	if (type.raw_type == RawType::VEC) {
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
	jit_type_t jit_value_type = VM::get_jit_type(value_type);
	jit_value_t value_v = jit_value_create(c.F, jit_value_type);
	jit_type_t jit_key_type = VM::get_jit_type(key_type);
	jit_value_t key_v = key ? jit_value_create(c.F, jit_key_type) : nullptr;

	c.add_var(value->content, value_v, value_type, true);
	if (key) c.add_var(key->content, key_v, key_type, true);


	jit_label_t label_end = jit_label_undefined;
	jit_label_t label_it = jit_label_undefined;
	c.enter_loop(&label_end, &label_it);

	// Static Selector
	if (container->type == Type(RawType::VEC, { Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int, (void*) fun_key_array_int, (void*) fun_inc_array_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::VEC, { Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float, (void*) fun_key_array_float, (void*) fun_inc_array_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::VEC, { Type::VAR })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr, (void*) fun_inc_array_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::VAR, Type::VAR })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_ptr, (void*) fun_key_map_ptr_ptr, (void*) fun_inc_map_ptr_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::VAR, Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_int, (void*) fun_key_map_ptr_int, (void*) fun_inc_map_ptr_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::VAR, Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_float, (void*) fun_key_map_ptr_float, (void*) fun_inc_map_ptr_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::I32, Type::VAR })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_ptr, (void*) fun_key_map_int_ptr, (void*) fun_inc_map_int_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::I32, Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_int, (void*) fun_key_map_int_int, (void*) fun_inc_map_int_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::MAP, { Type::I32, Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_float, (void*) fun_key_map_int_float, (void*) fun_inc_map_int_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::SET, { Type::I32 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<int>, (void*) fun_key_set<int>, (void*) fun_inc_set<int>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::SET, { Type::F64 })) {
		compile_foreach(c, container_v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<double>, (void*) fun_key_set<double>, (void*) fun_inc_set<double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (container->type == Type(RawType::SET, { Type::VAR })) {
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
