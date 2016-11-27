#include "Array.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

using namespace std;

namespace ls {

Array::Array() {
	type = Type::PTR_ARRAY;
}

Array::~Array() {
	for (auto ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug) const {

	if (interval) {
		os << "[";
		expressions[0]->print(os, indent, debug);
		os << "..";
		expressions[1]->print(os, indent, debug);
		os << "]";
	} else {
		os << "[";
		for (size_t i = 0; i < expressions.size(); ++i) {
			expressions[i]->print(os, indent, debug);
			if (i < expressions.size() - 1) {
				os << ", ";
			}
		}
		os << "]";
	}

	if (debug) {
		os << " " << type;
	}
}

unsigned Array::line() const {
	return 0;
}

void Array::analyse(SemanticAnalyser* analyser, const Type&) {

	constant = true;

	if (interval) {

		type = Type::INTERVAL;
		expressions[0]->analyse(analyser, Type::INTEGER);
		expressions[1]->analyse(analyser, Type::INTEGER);

	} else {

		if (expressions.size() > 0) {

			Type element_type = Type::UNKNOWN;
			Type supported_type = Type::UNKNOWN;

			for (size_t i = 0; i < expressions.size(); ++i) {

				Value* ex = expressions[i];
				ex->analyse(analyser, Type::UNKNOWN);

				if (ex->constant == false) {
					constant = false;
				}
				element_type = Type::get_compatible_type(element_type, ex->type);
			}

			// Native elements types supported : integer, double
			if (element_type == Type::INTEGER || element_type == Type::REAL) {
				supported_type = element_type;
			}
			// For function, we store them as pointers
			else if (element_type.raw_type == RawType::FUNCTION) {
				element_type.nature = Nature::POINTER;
				supported_type = element_type;
			} else {
				supported_type = Type::POINTER;
				// If there are some functions, they types will be lost, so tell them to return pointers
				supported_type.setReturnType(Type::POINTER);
			}

			// Re-analyze expressions with the supported type
			for (size_t i = 0; i < expressions.size(); ++i) {
				expressions[i]->analyse(analyser, supported_type);
			}

			// Second computation of the array type
			element_type = Type::UNKNOWN;
			for (unsigned i = 0; i < expressions.size(); ++i) {
				Value* ex = expressions[i];
				element_type = Type::get_compatible_type(element_type, ex->type);
			}
			element_type.temporary = false;
			type.setElementType(element_type);
		}
	}
}

void Array::elements_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

//	cout << "Array::elements_will_take " << type << " at " << pos << endl;

	for (size_t i = 0; i < expressions.size(); ++i) {

		Array* arr = dynamic_cast<Array*>(expressions[i]);
		if (arr != nullptr && level > 0) {
			arr->elements_will_take(analyser, arg_types, level - 1);
		} else {
			expressions[i]->will_take(analyser, arg_types);
		}
	}

	// Computation of the new array type
	Type element_type;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = Type::get_compatible_type(element_type, ex->type);
		}
	}
	this->type.setElementType(element_type);

//	cout << "Array::elements_will_take type after " << this->type << endl;
}

bool Array::will_store(SemanticAnalyser* analyser, const Type& type) {

//	std::cout << "Array::will_store(" << type << ")" << std::endl;

	Type added_type = type;
	if (added_type.raw_type == RawType::ARRAY) {
		added_type = added_type.getElementType();
	}
	Type current_type = this->type.getElementType();
	if (expressions.size() == 0) {
		this->type.setElementType(added_type);
	} else {
		this->type.setElementType(Type::get_compatible_type(current_type, added_type));
	}
	// Re-analyze expressions with the new type
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->analyse(analyser, this->type.getElementType());
	}
	return false;
}

LSInterval* LSArray_create_interval(int a, int b) {
	LSInterval* interval = new LSInterval();
	interval->a = a;
	interval->b = b;
	return interval;
}

Compiler::value Array::compile(Compiler& c) const {

	if (interval) {

		auto a = expressions[0]->compile(c);
		auto b = expressions[1]->compile(c);

		jit_type_t args[2] = {LS_INTEGER, LS_INTEGER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 2, 0);
		jit_value_t args_v[] = {a.v, b.v};

		jit_value_t interval = jit_insn_call_native(c.F, "new", (void*) LSArray_create_interval, sig, args_v, 2, JIT_CALL_NOTHROW);

		return {interval, Type::INTERVAL};
	}

	jit_value_t array = VM::create_array(c.F, type.getElementType(), expressions.size());

	for (Value* val : expressions) {

		auto v = val->compile(c);
		VM::push_move_array(c.F, type.getElementType(), array, v.v);
	}

	// size of the array + 1 operations
	VM::inc_ops(c.F, expressions.size() + 1);

	return {array, type};
}

}
