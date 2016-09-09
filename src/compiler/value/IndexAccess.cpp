#include "IndexAccess.hpp"
#include "Array.hpp"
#include "../../vm/value/LSVec.hpp"
#include "../../vm/value/LSMap.hpp"
#include "../../vm/value/LSVar.hpp"

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
	if (key2) key2->analyse(analyser);

	if (container->type.get_raw_type() == &RawType::VEC) {
		left_type = container->type.element_type(0);
	} else if (container->type.get_raw_type() == &RawType::MAP) {
		left_type = container->type.element_type(1);
	} else {
		left_type = Type::UNKNOWN;
	}

	type = left_type.image_conversion();
}

void IndexAccess::reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type, const Type& req_left_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	if (!Type::intersection(left_type, req_left_type, &left_type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}


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
			if (key2) key2->reanalyse(analyser, Type::I32);
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			element_type = container->type.element_type(1);
			Type key_type = container->type.element_type(0);
			key->reanalyse(analyser, key_type);
			if (key2) key2->reanalyse(analyser, key_type);
		} else {
			element_type = Type::UNKNOWN;
			key->reanalyse(analyser, Type::UNKNOWN);
			if (key2) key2->reanalyse(analyser, Type::UNKNOWN);
		}
	}

	if (!Type::intersection(type, element_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
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
			if (key2) key2->finalize(analyser, Type::I32);
		} else if (container->type.get_raw_type() == &RawType::MAP) {
			element_type = container->type.element_type(1);
			Type key_type = container->type.element_type(0);
			key->finalize(analyser, key_type);
			if (key2) key2->finalize(analyser, key_type);
		} else {
			assert(0);
		}
	}

	if (!Type::intersection(type, element_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();

	assert(type.is_pure() || !analyser->errors.empty());
}



LSValue* IA_vec_lsptr(LSVec<LSValue*>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) return nullptr;
	if (i >= vec->size()) {
		if (vec->refs == 0) delete vec;
		return nullptr;
	}
	LSValue* r = (*vec)[i];
	if (vec->refs == 0) {
		(*vec)[i] = nullptr;
		delete vec;
		r->refs--;
	}
	return r;
}
template <typename T>
T IA_vec(LSVec<T>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) return T(0);
	T r = (i < vec->size()) ? (*vec)[i] : T(0);
	if (vec->refs == 0) delete vec;
	return r;
}

jit_value_t IndexAccess::compile(Compiler& c) const {
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	if (!key2) {
		jit_value_t val = nullptr;

		// VEC
		if (container->type.raw_type == &RawType::VEC) {
			if (container->type.element_type(0).raw_type->nature() == Nature::LSVALUE) val = Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_vec_lsptr, { a, k });
			if (container->type.element_type(0) == Type::BOOLEAN)                      val = Compiler::call_native(c.F, LS_I32,     { LS_POINTER, LS_I32 }, (void*) IA_vec<int32_t>, { a, k });
			if (container->type.element_type(0) == Type::I32)                          val = Compiler::call_native(c.F, LS_I32,     { LS_POINTER, LS_I32 }, (void*) IA_vec<int32_t>, { a, k });
			if (container->type.element_type(0) == Type::F64)                          val = Compiler::call_native(c.F, LS_F64,     { LS_POINTER, LS_I32 }, (void*) IA_vec<double>, { a, k });
			if (container->type.element_type(0).raw_type == &RawType::FUNCTION)        val = Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_vec<void*>, { a, k });
			// TODO tuple
			assert(val);
		}

		return Compiler::compile_convert(c.F, val, container->type.element_type(0), type);

	} else {
		//jit_value_t k2 = key2->compile(c);
		// TODO
		assert(0);
	}
	return nullptr;
}

LSValue** IA_l_vec_lsptr(LSVec<LSValue*>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) {
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}
template <typename T>
T* IA_l_vec(LSVec<T>* vec, uint32_t i) {
	VM::operations += 2;
	if (vec == nullptr) {
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}

jit_value_t IndexAccess::compile_l(Compiler& c) const
{
	jit_value_t a = container->compile(c);
	jit_value_t k = key->compile(c);

	// VEC
	if (container->type.raw_type == &RawType::VEC) {
		if (container->type.element_type(0).raw_type->nature() == Nature::LSVALUE) return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec_lsptr, { a, k });
		if (container->type.element_type(0) == Type::BOOLEAN)                      return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<int32_t>, { a, k });
		if (container->type.element_type(0) == Type::I32)                          return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<int32_t>, { a, k });
		if (container->type.element_type(0) == Type::F64)                          return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<double>, { a, k });
		if (container->type.element_type(0).raw_type == &RawType::FUNCTION)        return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<void*>, { a, k });
		// TODO tuple
		assert(0);
	}
	return nullptr;
}

}
