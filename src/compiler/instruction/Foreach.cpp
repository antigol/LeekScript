#include "../../compiler/instruction/Foreach.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
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

	if (req_type.raw_type == RawType::ARRAY && req_type.nature == Nature::POINTER) {
		type = req_type;
	} else {
		type = Type::VOID;
	}

	analyser->enter_block();

	container->analyse(analyser, Type::UNKNOWN);

	//if (not container->iterable()) {
		// TODO
		// analyser->add_error();
	//}

	if (container->type.element_type.size() == 1) {

		key_type = Type::INTEGER; // If no key type in array key = 0, 1, 2...
		value_type = container->type.getElementType();

	} else if (container->type.raw_type == RawType::MAP) {

		key_type = container->type.getKeyType();
		value_type = container->type.getElementType();

	} else if (container->type.raw_type == RawType::ARRAY ||
			   container->type.raw_type == RawType::SET) {
		key_type = Type::INTEGER;
		value_type = Type::POINTER;
	} else {
		key_type = Type::POINTER;
		value_type = Type::POINTER;
	}

	if (key != nullptr) {
		key_var = analyser->add_var(key, key_type, nullptr, nullptr);
	}
	value_var = analyser->add_var(value, value_type, nullptr, nullptr);


	analyser->enter_loop();
	if (type == Type::VOID) {
		body->analyse(analyser, Type::VOID);
	} else {
		body->analyse(analyser, type.getElementType());
		type.setElementType(body->type);
	}
	analyser->leave_loop();
	analyser->leave_block();
}

/*
 * Begin
 */
LSArray<LSValue*>::iterator fun_begin_array_all(LSArray<LSValue*>* array) {
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
bool fun_condition_array_all(LSArray<LSValue*>* array, LSArray<LSValue*>::iterator it) {
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
LSValue* fun_value_array_ptr(LSArray<LSValue*>::iterator it) {
	return *it;
}
int fun_value_array_int(LSArray<int>::iterator it) {
	return *it;
}
LSValue* fun_value_array_int_2ptr(LSArray<int>::iterator it) {
	return LSNumber::get(*it);
}
double fun_value_array_float(LSArray<double>::iterator it) {
	return *it;
}
LSValue* fun_value_array_float_2ptr(LSArray<double>::iterator it) {
	return LSNumber::get(*it);
}
LSValue* fun_value_map_ptr_ptr(LSMap<LSValue*,LSValue*>::iterator it) {
	return it->second;
}
int fun_value_map_ptr_int(LSMap<LSValue*,int>::iterator it) {
	return it->second;
}
LSValue* fun_value_map_ptr_int_2ptr(LSMap<LSValue*,int>::iterator it) {
	return LSNumber::get(it->second);
}
double fun_value_map_ptr_float(LSMap<LSValue*,double>::iterator it) {
	return it->second;
}
LSValue* fun_value_map_ptr_float_2ptr(LSMap<LSValue*,double>::iterator it) {
	return LSNumber::get(it->second);
}
LSValue* fun_value_map_int_ptr(LSMap<int,LSValue*>::iterator it) {
	return it->second;
}
int fun_value_map_int_int(LSMap<int,int>::iterator it) {
	return it->second;
}
LSValue* fun_value_map_int_int_2ptr(LSMap<int,int>::iterator it) {
	return LSNumber::get(it->second);
}
double fun_value_map_int_float(LSMap<int,double>::iterator it) {
	return it->second;
}
LSValue* fun_value_map_int_float_2ptr(LSMap<int,double>::iterator it) {
	return LSNumber::get(it->second);
}
template <typename T>
T fun_value_set(typename LSSet<T>::iterator it) {
	return *it;
}
template <typename T>
LSValue* fun_value_set_2ptr(typename LSSet<T>::iterator it) {
	return LSNumber::get(*it);
}
//int fun_value_set_int(LSSet<int>::iterator it) {
//	return *it;
//}
//LSValue* fun_value_set_int_2ptr(LSSet<int>::iterator it) {
//	return LSNumber::get(*it);
//}
//double fun_value_set_float(LSSet<double>::iterator it) {
//	return *it;
//}
//LSValue* fun_value_set_float_2ptr(LSSet<double>::iterator it) {
//	return LSNumber::get(*it);
//}


/*
 * Key
 */
int fun_key_array_ptr(LSArray<LSValue*>* array, LSArray<LSValue*>::iterator it) {
	return distance(array->begin(), it);
}
int fun_key_array_int(LSArray<int>* array, LSArray<int>::iterator it) {
	return distance(array->begin(), it);
}
int fun_key_array_float(LSArray<double>* array, LSArray<double>::iterator it) {
	return distance(array->begin(), it);
}
LSValue* fun_key_array_ptr_2ptr(LSArray<LSValue*>* array, LSArray<LSValue*>::iterator it) {
	return LSNumber::get(distance(array->begin(), it));
}
LSValue* fun_key_array_int_2ptr(LSArray<int>* array, LSArray<int>::iterator it) {
	return LSNumber::get(distance(array->begin(), it));
}
LSValue* fun_key_array_float_2ptr(LSArray<double>* array, LSArray<double>::iterator it) {
	return LSNumber::get(distance(array->begin(), it));
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
LSValue* fun_key_map_int_ptr_2ptr(void*, LSMap<int,LSValue*>::iterator it) {
	return LSNumber::get(it->first);
}
LSValue* fun_key_map_int_int_2ptr(void*, LSMap<int,int>::iterator it) {
	return LSNumber::get(it->first);
}
LSValue* fun_key_map_int_float_2ptr(void*, LSMap<int,double>::iterator it) {
	return LSNumber::get(it->first);
}
template <typename T>
int fun_key_set(LSSet<T>* set, typename LSSet<T>::iterator it) {
	return distance(set->begin(), it);
}
template <typename T>
LSValue* fun_key_set_2ptr(LSSet<T>* set, typename LSSet<T>::iterator it) {
	return LSNumber::get(distance(set->begin(), it));
}



/*
 * Increment
 */
LSArray<LSValue*>::iterator fun_inc_array_ptr(LSArray<LSValue*>::iterator it) {
	return ++it;
}
LSArray<int>::iterator fun_inc_array_int(LSArray<int>::iterator it) {
	return ++it;
}
LSArray<double>::iterator fun_inc_array_float(LSArray<double>::iterator it) {
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


/*
 * Dynamic selector
 */
enum SelectorType {
	PTR_ARRAY = 0,
	INT_ARRAY,
	REAL_ARRAY,
	PTR_PTR_MAP,
	PTR_INT_MAP,
	PTR_REAL_MAP,
	INT_PTR_MAP,
	INT_INT_MAP,
	INT_REAL_MAP,
	PTR_SET,
	INT_SET,
	REAL_SET,
	OTHER_NOT_SUPPORTED
};
int fun_selector(LSValue* containter) {
	if (dynamic_cast<LSArray<LSValue*>*>(containter)) {
		return SelectorType::PTR_ARRAY;
	}
	if (dynamic_cast<LSArray<int>*>(containter)) {
		return SelectorType::INT_ARRAY;
	}
	if (dynamic_cast<LSArray<double>*>(containter)) {
		return SelectorType::REAL_ARRAY;
	}
	if (dynamic_cast<LSMap<LSValue*,LSValue*>*>(containter)) {
		return SelectorType::PTR_PTR_MAP;
	}
	if (dynamic_cast<LSMap<LSValue*,int>*>(containter)) {
		return SelectorType::PTR_INT_MAP;
	}
	if (dynamic_cast<LSMap<LSValue*,double>*>(containter)) {
		return SelectorType::PTR_REAL_MAP;
	}
	if (dynamic_cast<LSMap<int,LSValue*>*>(containter)) {
		return SelectorType::INT_PTR_MAP;
	}
	if (dynamic_cast<LSMap<int,int>*>(containter)) {
		return SelectorType::INT_INT_MAP;
	}
	if (dynamic_cast<LSMap<int,double>*>(containter)) {
		return SelectorType::INT_REAL_MAP;
	}
	if (dynamic_cast<LSSet<LSValue*>*>(containter)) {
		return SelectorType::PTR_SET;
	}
	if (dynamic_cast<LSSet<int>*>(containter)) {
		return SelectorType::INT_SET;
	}
	if (dynamic_cast<LSSet<double>*>(containter)) {
		return SelectorType::REAL_SET;
	}

	return SelectorType::OTHER_NOT_SUPPORTED;
}

Compiler::value Foreach::compile(Compiler& c) const {

	c.enter_block(); // { for x in [1, 2] {} }<-- this block

	// Potential output [for ...]
	jit_value_t output_v = nullptr;
	if (type.raw_type == RawType::ARRAY && type.nature == Nature::POINTER) {
		output_v = VM::create_array(c.F, type.getElementType());
		VM::inc_refs(c.F, output_v);
		c.add_var("{output}", output_v, type, false); // Why create variable ? in case of `break 2` the output must be deleted
	}

	// Container (Array, Map or Set)
	auto container_v = container->compile(c);

	if (container->type.must_manage_memory()) {
		VM::inc_refs(c.F, container_v.v);
	}

	c.add_var("{array}", container_v.v, container->type, false);

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
	if (equal_type(container->type, Type::INT_ARRAY)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int, (void*) fun_key_array_int, (void*) fun_inc_array_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::REAL_ARRAY)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float, (void*) fun_key_array_float, (void*) fun_inc_array_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::PTR_ARRAY)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr, (void*) fun_inc_array_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::PTR_PTR_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_ptr, (void*) fun_key_map_ptr_ptr, (void*) fun_inc_map_ptr_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::PTR_INT_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_int, (void*) fun_key_map_ptr_int, (void*) fun_inc_map_ptr_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::PTR_REAL_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_float, (void*) fun_key_map_ptr_float, (void*) fun_inc_map_ptr_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::INT_PTR_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_ptr, (void*) fun_key_map_int_ptr, (void*) fun_inc_map_int_ptr,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::INT_INT_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_int, (void*) fun_key_map_int_int, (void*) fun_inc_map_int_int,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::INT_REAL_MAP)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_float, (void*) fun_key_map_int_float, (void*) fun_inc_map_int_float,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::INT_SET)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<int>, (void*) fun_key_set<int>, (void*) fun_inc_set<int>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::REAL_SET)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<double>, (void*) fun_key_set<double>, (void*) fun_inc_set<double>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);
	} else if (equal_type(container->type, Type::PTR_SET)) {
		compile_foreach(c, container_v.v, output_v,
						(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<LSValue*>, (void*) fun_key_set<LSValue*>, (void*) fun_inc_set<LSValue*>,
						&label_it, &label_end, jit_value_type, value_v, jit_key_type, key_v);

	} else if (container->type.nature == Nature::POINTER) {

		// Dynamic selector
		jit_type_t args_types_sel[1] = {LS_POINTER};
		jit_type_t sig_sel = jit_type_create_signature(jit_abi_cdecl, jit_type_int, args_types_sel, 1, 0);
		jit_value_t selector_v = jit_insn_call_native(c.F, "selector", (void*) fun_selector, sig_sel, &container_v.v, 1, JIT_CALL_NOTHROW);

		// Goto begin
		jit_label_t label_begin = jit_label_undefined;
		jit_insn_branch(c.F, &label_begin);

		// Body
		jit_label_t label_block = jit_label_undefined;
		jit_insn_label(c.F, &label_block);
		auto body_v = body->compile(c);
		if (output_v && body_v.v) {
			VM::push_move_array(c.F, type.getElementType(), output_v, body_v.v);
		}
		jit_insn_label(c.F, &label_it);


		if (container->type == Type::PTR_ARRAY) {

			jit_label_t destinations_begin[3] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};
			jit_label_t destinations_it[3] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};

			jit_insn_jump_table(c.F, selector_v, destinations_it, 3);

			// Begin
			jit_insn_label(c.F, &label_begin);
			jit_insn_jump_table(c.F, selector_v, destinations_begin, 3);
			jit_insn_branch(c.F, &label_end); // by default (not realy needed for the array case)


			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr, (void*) fun_inc_array_ptr,
									&destinations_begin[0], &destinations_it[0], &label_block, &label_end, jit_value_type, value_v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int_2ptr, (void*) fun_key_array_int, (void*) fun_inc_array_int,
									&destinations_begin[1], &destinations_it[1], &label_block, &label_end, jit_value_type, value_v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float_2ptr, (void*) fun_key_array_float, (void*) fun_inc_array_float,
									&destinations_begin[2], &destinations_it[2], &label_block, &label_end, jit_value_type, value_v, jit_key_type, key_v);

		} else {

			jit_label_t destinations_begin[12] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};
			jit_label_t destinations_it[12] = {
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
				jit_label_undefined, jit_label_undefined, jit_label_undefined,
			};

			jit_insn_jump_table(c.F, selector_v, destinations_it, 12);

			// Begin
			jit_insn_label(c.F, &label_begin);
			jit_insn_jump_table(c.F, selector_v, destinations_begin, 12);
			jit_insn_branch(c.F, &label_end); // by default


			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_ptr, (void*) fun_key_array_ptr_2ptr, (void*) fun_inc_array_ptr,
									&destinations_begin[0], &destinations_it[0], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_int_2ptr, (void*) fun_key_array_int_2ptr, (void*) fun_inc_array_int,
									&destinations_begin[1], &destinations_it[1], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_array_all, (void*) fun_condition_array_all, (void*) fun_value_array_float_2ptr, (void*) fun_key_array_float_2ptr, (void*) fun_inc_array_float,
									&destinations_begin[2], &destinations_it[2], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);

			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_ptr, (void*) fun_key_map_ptr_ptr, (void*) fun_inc_map_ptr_ptr,
									&destinations_begin[3], &destinations_it[3], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_int_2ptr, (void*) fun_key_map_ptr_int, (void*) fun_inc_map_ptr_int,
									&destinations_begin[4], &destinations_it[4], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_ptr_float, (void*) fun_key_map_ptr_float, (void*) fun_inc_map_ptr_float,
									&destinations_begin[5], &destinations_it[5], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_ptr, (void*) fun_key_map_int_ptr_2ptr, (void*) fun_inc_map_int_ptr,
									&destinations_begin[6], &destinations_it[6], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_int_2ptr, (void*) fun_key_map_int_int_2ptr, (void*) fun_inc_map_int_int,
									&destinations_begin[7], &destinations_it[7], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_map_all, (void*) fun_condition_map_all, (void*) fun_value_map_int_float_2ptr, (void*) fun_key_map_int_float_2ptr, (void*) fun_inc_map_int_float,
									&destinations_begin[8], &destinations_it[8], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);

			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set<LSValue*>, (void*) fun_key_set_2ptr<LSValue*>, (void*) fun_inc_set<LSValue*>,
									&destinations_begin[9], &destinations_it[9], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set_2ptr<int>, (void*) fun_key_set_2ptr<int>, (void*) fun_inc_set<int>,
									&destinations_begin[10], &destinations_it[10], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
			compile_foreach_noblock(c, container_v.v,
									(void*) fun_begin_set_all, (void*) fun_condition_set_all, (void*) fun_value_set_2ptr<double>, (void*) fun_key_set_2ptr<double>, (void*) fun_inc_set<double>,
									&destinations_begin[11], &destinations_it[11], &label_block, &label_end, jit_value_type, body_v.v, jit_key_type, key_v);
		}

	}

	c.leave_loop();

	// end label:
	jit_insn_label(c.F, &label_end);

	jit_value_t return_v = VM::clone_obj(c.F, output_v); // otherwise it is delete by the c.leave_block
	c.leave_block(c.F); // { for x in ['a' 'b'] { ... }<--- not this block }<--- this block

	return {return_v, type};
}

void Foreach::compile_foreach(Compiler&c, jit_value_t container_v, jit_value_t output_v,
							  void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
							  jit_label_t* label_it, jit_label_t* label_end,
							  jit_type_t jit_value_type, jit_value_t value_v, jit_type_t jit_key_type, jit_value_t key_v) const {

	// Labels
	jit_label_t label_cond = jit_label_undefined;

	// Variable it = begin()

	jit_value_t it_v = jit_value_create(c.F, LS_POINTER);
	jit_type_t args_types_begin[1] = {LS_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types_begin, 1, 0);
	jit_insn_store(c.F, it_v, jit_insn_call_native(c.F, "begin", (void*) fun_begin, sig_begin, &container_v, 1, JIT_CALL_NOTHROW));

	// cond label:
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	jit_type_t args_types_cond[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig_cond = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types_cond, 2, 0);
	jit_value_t args_cond[2] = { container_v, it_v };
	jit_value_t cond_v = jit_insn_call_native(c.F, "cond", (void*) fun_condition, sig_cond, args_cond, 2, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(c.F, cond_v, label_end);

	// Get Value
	jit_type_t args_types_value[1] = {LS_POINTER};
	jit_type_t sig_value = jit_type_create_signature(jit_abi_cdecl, jit_value_type, args_types_value, 1, 0);
	jit_value_t vtmp = jit_insn_call_native(c.F, "value", fun_value, sig_value, &it_v, 1, JIT_CALL_NOTHROW);
	jit_insn_store(c.F, value_v, vtmp);

	// Get Key
	if (key != nullptr) {
		jit_type_t args_types_key[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig_key = jit_type_create_signature(jit_abi_cdecl, jit_key_type, args_types_key, 2, 0);
		jit_value_t args_key[2] = { container_v, it_v };
		jit_value_t ktmp = jit_insn_call_native(c.F, "key", (void*) fun_key, sig_key, args_key, 2, JIT_CALL_NOTHROW);
		jit_insn_store(c.F, key_v, ktmp);
	}

	// Body
	auto body_v = body->compile(c);
	if (output_v && body_v.v) {
		VM::push_move_array(c.F, type.getElementType(), output_v, body_v.v);
	}

	// it++
	jit_insn_label(c.F, label_it);

	jit_type_t args_types_inc[1] = {LS_POINTER};
	jit_type_t sig_inc = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types_inc, 1, 0);
	jit_insn_store(c.F, it_v, jit_insn_call_native(c.F, "inc", fun_inc, sig_inc, &it_v, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);
}

void Foreach::compile_foreach_noblock(Compiler& c, jit_value_t container,
	void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
	jit_label_t* label_begin, jit_label_t* label_it, jit_label_t* label_block, jit_label_t* label_end,
	jit_type_t jit_value_type, jit_value_t v, jit_type_t jit_key_type, jit_value_t k) const {

	// begin label
	jit_insn_label(c.F, label_begin);

	// Variable it = begin()
	jit_value_t it = jit_value_create(c.F, LS_POINTER);
	jit_type_t args_types_begin[1] = {LS_POINTER};
	jit_type_t sig_begin = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types_begin, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "begin", (void*) fun_begin, sig_begin, &container, 1, JIT_CALL_NOTHROW));

	// cond label:
	jit_label_t label_cond = jit_label_undefined;
	jit_insn_label(c.F, &label_cond);

	// Condition to continue
	jit_type_t args_types_cond[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig_cond = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types_cond, 2, 0);
	jit_value_t args_cond[2] = { container, it };
	jit_value_t cond = jit_insn_call_native(c.F, "cond", (void*) fun_condition, sig_cond, args_cond, 2, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(c.F, cond, label_end);

	// Get Value
	jit_type_t args_types_value[1] = {LS_POINTER};
	jit_type_t sig_value = jit_type_create_signature(jit_abi_cdecl, jit_value_type, args_types_value, 1, 0);
	jit_value_t vtmp = jit_insn_call_native(c.F, "value", fun_value, sig_value, &it, 1, JIT_CALL_NOTHROW);
	jit_insn_store(c.F, v, vtmp);

	// Get Key
	if (key != nullptr) {
		jit_type_t args_types_key[2] = {LS_POINTER, LS_POINTER};
		jit_type_t sig_key = jit_type_create_signature(jit_abi_cdecl, jit_key_type, args_types_key, 2, 0);
		jit_value_t args_key[2] = { container, it };
		jit_value_t ktmp = jit_insn_call_native(c.F, "key", (void*) fun_key, sig_key, args_key, 2, JIT_CALL_NOTHROW);
		jit_insn_store(c.F, k, ktmp);
	}

	// Body
	jit_insn_branch(c.F, label_block);

	// it++
	jit_insn_label(c.F, label_it);
	jit_type_t args_types_inc[1] = {LS_POINTER};
	jit_type_t sig_inc = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types_inc, 1, 0);
	jit_insn_store(c.F, it, jit_insn_call_native(c.F, "inc", fun_inc, sig_inc, &it, 1, JIT_CALL_NOTHROW));

	// jump to cond
	jit_insn_branch(c.F, &label_cond);
}

bool Foreach::equal_type(const Type& generic, const Type& actual) {

	if (generic.raw_type != actual.raw_type) return false;
	if (generic.nature != actual.nature) return false;
	if (generic.element_type.size() != actual.element_type.size()) return false;

	for (size_t i = 0; i < generic.element_type.size(); ++i) {
		const Type& a = generic.element_type[i];
		const Type& b = actual.element_type[i];
		if (a.nature != b.nature) return false;
		if (a.nature == Nature::VALUE && a.raw_type != b.raw_type) return false;
	}

	return true;
}

}
