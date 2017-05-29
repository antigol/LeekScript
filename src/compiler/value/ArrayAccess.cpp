#include "ArrayAccess.hpp"
#include "../../compiler/value/Array.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSInterval.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

ArrayAccess::ArrayAccess() {
	array = nullptr;
	key = nullptr;
	key2 = nullptr;
	type = Type::POINTER;
}

ArrayAccess::~ArrayAccess() {
	delete array;
	delete key;
	if (key2 != nullptr) {
		delete key2;
	}
}

void ArrayAccess::print(std::ostream& os, int indent, bool debug) const {
	array->print(os, indent, debug);
	os << "[";
	if (key != nullptr) {
		key->print(os, indent, debug);
	}
	if (key2 != nullptr) {
		os << ":";
		key2->print(os, indent, debug);
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

Location ArrayAccess::location() const {
	return {array->location().start, close_bracket->location.end};
}

void ArrayAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	array->analyse(analyser, Type::UNKNOWN);

	if (array->type.raw_type != RawType::UNKNOWN and !array->type.is_container()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, location(), array->location(), {array->to_string()}});
		return;
	}

	if (key == nullptr) {
		return;
	}

	key->analyse(analyser, Type::UNKNOWN);
	constant = array->constant && key->constant;

	array_element_type = Type::UNKNOWN;
	if (array->type.raw_type == RawType::ARRAY || array->type.raw_type == RawType::INTERVAL
		|| array->type.raw_type == RawType::MAP) {
		array_element_type = array->type.getElementType();
		type = array_element_type;
	}
	if (array->type.raw_type == RawType::MAP) {
		map_key_type = array->type.getKeyType();
	}

	if (array->type == Type::INTERVAL) {
		key->analyse(analyser, Type::INTEGER);
	}

	// Range array access : array[4:12], check if the values are numbers
	if (key != nullptr and key2 != nullptr) {

		key->analyse(analyser, Type::INTEGER);
		key2->analyse(analyser, Type::INTEGER);

		if (key->type != Type::UNKNOWN and not key->type.isNumber()) {
			std::string k = "<key 1>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key->location(), {k}});
		}
		if (key2->type != Type::UNKNOWN and not key2->type.isNumber()) {
			std::string k = "<key 2>";
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER, location(), key2->location(), {k}});
		}
		type = array->type;

	} else if (array->type.raw_type == RawType::ARRAY or array->type.raw_type == RawType::STRING
		or array->type.raw_type == RawType::INTERVAL) {

		if (key->type.raw_type != RawType::UNKNOWN and not (key->type.isNumber() or key->type.raw_type == RawType::BOOLEAN)) {
			std::string a = array->to_string();
			std::string k = key->to_string();
			std::string kt = key->type.to_string();
			analyser->add_error({SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, location(), key->location(), {k, a, kt}});
		}
		key->analyse(analyser, Type::INTEGER);

		if (array->type.raw_type == RawType::STRING) {
			type = Type::STRING;
		}

	} else if (array->type.raw_type == RawType::MAP) {

		key->analyse(analyser, Type::UNKNOWN);

		if (map_key_type == Type::INTEGER) {
			key->analyse(analyser, Type::INTEGER);
		} else if (map_key_type == Type::REAL) {
			key->analyse(analyser, Type::REAL);
		} else {
			key->analyse(analyser, Type::POINTER);
		}

		if (map_key_type != Type::POINTER) {
			if (map_key_type.nature == Nature::VALUE and key->type.nature == Nature::POINTER) {
				std::string a = array->to_string();
				std::string k = key->to_string();
				std::string kt = key->type.to_string();
				analyser->add_error({SemanticError::Type::INVALID_MAP_KEY, location(), key->location(), {k, a, kt}});
			}
		}

	} else {
		key->analyse(analyser, Type::POINTER);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

bool ArrayAccess::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {

	// std::cout << "ArrayAccess::will_take(" << args << ", " << level << ")" << std::endl;

	type.will_take(args);

	if (Array* arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, args, 1);
	}
	if (ArrayAccess* arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, args, 1);
	}

	type = array->type.getElementType();

	return false;
}

bool ArrayAccess::array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

	type.will_take(arg_types);

	if (auto arr = dynamic_cast<Array*>(array)) {
		arr->elements_will_take(analyser, arg_types, level);
	}
	if (auto arr = dynamic_cast<ArrayAccess*>(array)) {
		arr->array_access_will_take(analyser, arg_types, level + 1);
	}

	type = array->type.getElementType();

	return false;
}

void ArrayAccess::change_type(SemanticAnalyser* analyser, const Type& new_type) {
	if (Type::more_specific(new_type, type)) {
		array->will_store(analyser, new_type);
		this->type = array->type.getElementType();
		this->types = this->type;
		array_element_type = this->type;
	}
}

int interval_access(const LSInterval* interval, int pos) {
	return interval->atv(pos);
}

Compiler::value ArrayAccess::compile(Compiler& c) const {

	jit_insn_mark_offset(c.F, open_bracket->location.start.line);

	((ArrayAccess*) this)->compiled_array = array->compile(c);
	array->compile_end(c);

	c.inc_ops(2); // Array access : 2 operations

	if (key2 == nullptr) {

		if (array->type == Type::INTERVAL) {

			jit_type_t args_types[2] = {LS_POINTER, LS_INTEGER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 2, 1);

			auto k = key->compile(c);
			key->compile_end(c);

			jit_value_t args[] = {compiled_array.v, k.v};
			jit_value_t res = jit_insn_call_native(c.F, "access", (void*) interval_access, sig, args, 2, 0);
			jit_type_free(sig);

			if (type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, res, type), type};
			}
			return {res, type};

		} else if (array->type.raw_type == RawType::MAP) {

			jit_type_t args_types[2] = {LS_POINTER, VM::get_jit_type(map_key_type)};

			auto k = key->compile(c);
			key->compile_end(c);

			void* func = nullptr;
			if (map_key_type == Type::INTEGER) {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<int, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<int, double>::at;
				} else {
					func = (void*) &LSMap<int, LSValue*>::at;
				}
			} else if (map_key_type == Type::REAL) {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<double, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<double, double>::at;
				} else {
					func = (void*) &LSMap<double, LSValue*>::at;
				}
			} else {
				if (array_element_type == Type::INTEGER) {
					func = (void*) &LSMap<LSValue*, int>::at;
				} else if (array_element_type == Type::REAL) {
					func = (void*) &LSMap<LSValue*, double>::at;
				} else {
					func = (void*) &LSMap<LSValue*, LSValue*>::at;
				}
			}

			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(array_element_type), args_types, 2, 1);
			jit_value_t args[] = {compiled_array.v, k.v};
			jit_value_t res = jit_insn_call_native(c.F, "access", func, sig, args, 2, 0);
			jit_type_free(sig);

			// if (key->type.must_manage_memory()) {
				c.insn_delete_temporary(k);
			// }

			c.inc_ops(2);

			if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, res, type), type};
			}
			return {res, type};

		} else if (array->type.raw_type == RawType::STRING or array->type.raw_type == RawType::ARRAY) {

			auto k = key->compile(c);
			key->compile_end(c);

			if (k.t.nature == Nature::POINTER) {
				k = c.insn_call(Type::INTEGER, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key_pointer) {
					auto n = dynamic_cast<LSNumber*>(key_pointer);
					if (!n) {
						LSValue::delete_temporary(array);
						LSValue::delete_temporary(key_pointer);
						jit_exception_throw(new vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER));
					}
					int key_int = n->value;
					LSValue::delete_temporary(key_pointer);
					return key_int;
				});
			}
			k = c.to_int(k);

			// Check index : k < 0 or k >= size
			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});

			if (array->type.raw_type == RawType::STRING) {
				auto e = c.insn_call(Type::STRING, {compiled_array, k}, (void*) &LSString::codePointAt);
				return e;
			} else {
				auto e = c.insn_load(c.insn_add(c.insn_load(compiled_array, 24, Type::POINTER), c.insn_mul(c.new_integer(array_element_type.size() / 8), k)), 0, array_element_type);
				e = c.clone(e);

				if (array_element_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, e.v, type), type};
				}
				return e;
			}
		} else {
			// Unknown type, call generic at() operator
			auto k = key->compile(c);
			key->compile_end(c);
			auto e = c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->at(key);
			});
			c.insn_delete_temporary(k);
			return e;
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		return c.insn_call(Type::POINTER, {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
			return a->range(start, end);
		});
	}
}

Compiler::value ArrayAccess::compile_l(Compiler& c) const {

	// Compile the array
	((ArrayAccess*) this)->compiled_array = [&]() {
		if (auto la = dynamic_cast<LeftValue*>(array)) {
			return c.insn_load(la->compile_l(c), 0, array->type);
		} else {
			return array->compile(c);
		}
	}();
	array->compile_end(c);

	if (key2 == nullptr) {
		// Compile the key
		auto k = key->compile(c);
		key->compile_end(c);
		// Access
		jit_insn_mark_offset(c.F, location().start.line);
		if (array->type.raw_type == RawType::ARRAY) {
			auto array_size = c.insn_array_size(compiled_array);
			c.insn_if(c.insn_or(c.insn_lt(k, c.new_integer(0)), c.insn_ge(k, array_size)), [&]() {
				c.insn_delete_temporary(compiled_array);
				c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
			});
			return c.insn_add(c.insn_load(compiled_array, 24, Type::POINTER), c.insn_mul(c.new_integer(array_element_type.size() / 8), k));

		} else if (array->type.raw_type == RawType::MAP) {

			if (array->type == Type::PTR_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, int>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::PTR_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, double>* map, LSValue* key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::REAL_PTR_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, LSValue*>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::REAL_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, int>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::REAL_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<double, double>* map, double key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::INT_PTR_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, LSValue*>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::INT_INT_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, int>* map, int key) {
					return map->atL_base(key);
				});
			} else if (array->type == Type::INT_REAL_MAP) {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<int, double>* map, int key) {
					return map->atL_base(key);
				});
			} else {
				return c.insn_call(Type::POINTER, {compiled_array, k}, (void*) +[](LSMap<LSValue*, LSValue*>* map, LSValue* key) {
					return map->atL_base(key);
				});
			}
		} else {
			if (k.t == Type::INTEGER) {
				k = {VM::value_to_pointer(c.F, k.v, Type::INTEGER), Type::POINTER};
			}
			return c.insn_call(type, {compiled_array, k}, (void*) +[](LSValue* array, LSValue* key) {
				return array->atL(key);
			});
		}
	} else {
		auto start = key->compile(c);
		auto end = key2->compile(c);
		key->compile_end(c);
		key2->compile_end(c);
		jit_insn_mark_offset(c.F, open_bracket->location.start.line);
		return c.insn_call(Type::POINTER, {compiled_array, start, end}, (void*) +[](LSValue* a, int start, int end) {
			// TODO
			a->rangeL(start, end);
		});
	}
}

void ArrayAccess::compile_end(Compiler& c) const {
	c.insn_delete_temporary(compiled_array);
}

Value* ArrayAccess::clone() const {
	auto aa = new ArrayAccess();
	aa->array = array->clone();
	aa->key = key->clone();
	aa->key2 = key2 ? key2->clone() : nullptr;
	aa->open_bracket = open_bracket;
	aa->close_bracket = close_bracket;
	return aa;
}

}
