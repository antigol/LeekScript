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

void IndexAccess::analyse_help(SemanticAnalyser* analyser)
{
	container->analyse(analyser);
	container->reanalyse(analyser, Type::INDEXABLE);

	key->analyse(analyser);
	if (key2) key2->analyse(analyser);

	left_type = Type::UNKNOWN;

	// VEC
	if (container->type.get_raw_type() == &RawType::VEC) {
		key->reanalyse(analyser, Type::I32);
		if (key2) key2->reanalyse(analyser, Type::I32);

		left_type = container->type.element_type(0);
	}

	// MAP
	if (container->type.get_raw_type() == &RawType::MAP) {
		key->reanalyse(analyser, container->type.element_type(0));
		if (key2) key2->reanalyse(analyser, container->type.element_type(0));

		left_type = container->type.element_type(1);
	}

	type = left_type.image_conversion();
}

void IndexAccess::reanalyse_l_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!isLeftValue()) {
		add_error(analyser, SemanticException::VALUE_MUST_BE_A_LVALUE);
		return;
	}

	Type container_req_type = Type({ Type(&RawType::VEC, { req_type }), Type(&RawType::MAP, { Type::UNKNOWN, req_type }) });

	((LeftValue*) container)->reanalyse_l(analyser, container_req_type);

	if (container->type.get_raw_type() == &RawType::VEC) {
		left_type = container->type.element_type(0);
	} else if (container->type.get_raw_type() == &RawType::MAP) {
		left_type = container->type.element_type(1);
	} else {
		if (!Type::intersection(left_type, req_type, &left_type)) {
			add_error(analyser, SemanticException::INFERENCE_TYPE_ERROR);
		}
	}

	if (!Type::intersection(type, left_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void IndexAccess::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	cout << "IA wr " << type << " + " << req_type << " = ";

	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	cout << type << endl;

	Type fiber = type.fiber_conversion();
	Type container_req_type = Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) });

	container->reanalyse(analyser, container_req_type);

	if (container->type.get_raw_type() == &RawType::VEC) {
		left_type = container->type.element_type(0);
		key->reanalyse(analyser, Type::I32);
		if (key2) key2->reanalyse(analyser, Type::I32);
	} else if (container->type.get_raw_type() == &RawType::MAP) {
		left_type = container->type.element_type(1);
		Type key_type = container->type.element_type(0);
		key->reanalyse(analyser, key_type);
		if (key2) key2->reanalyse(analyser, key_type);
	}

	if (!Type::intersection(type, left_type.image_conversion(), &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void IndexAccess::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type fiber = type.fiber_conversion();
	container->finalize(analyser, Type({ Type(&RawType::VEC, { fiber }), Type(&RawType::MAP, { Type::UNKNOWN, fiber }) }));

	// VEC
	if (container->type.raw_type == &RawType::VEC) {
		key->finalize(analyser, Type::I32);
		if (key2) key2->finalize(analyser, Type::I32);

		left_type = container->type.element_type(0);
	}

	// MAP
	if (container->type.raw_type == &RawType::MAP) {
		key->finalize(analyser, container->type.element_type(0));
		if (key2) key2->finalize(analyser, container->type.element_type(0));

		left_type = container->type.element_type(1);
	}

	type.make_it_pure();
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
			if (container->type.element_type(0) == Type::I32)                          val = Compiler::call_native(c.F, LS_I32,     { LS_POINTER, LS_I32 }, (void*) IA_vec<int32_t>, { a, k });
			if (container->type.element_type(0) == Type::F64)                          val = Compiler::call_native(c.F, LS_F64,     { LS_POINTER, LS_I32 }, (void*) IA_vec<double>, { a, k });
			if (container->type.element_type(0).raw_type == &RawType::FUNCTION)        val = Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_vec<void*>, { a, k });
			// TODO tuple
			assert(val);
		}

		return Compiler::compile_convert(c.F, val, container->type.element_type(0), type);

	} else {
		jit_value_t k2 = key2->compile(c);
	}
}

LSValue** IA_l_vec_lsptr(LSVec<LSValue*>* vec, int i) {
	VM::operations += 2;
	if (vec->refs == 0) {
		delete vec;
		return nullptr;
	}
	return i < vec->size() ? vec->data()+i : nullptr;
}
template <typename T>
T* IA_l_vec(LSVec<T>* vec, int i) {
	VM::operations += 2;
	if (vec->refs == 0) {
		delete vec;
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
		if (container->type.element_type(0) == Type::I32)                          return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<int32_t>, { a, k });
		if (container->type.element_type(0) == Type::F64)                          return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<double>, { a, k });
		if (container->type.element_type(0).raw_type == &RawType::FUNCTION)        return Compiler::call_native(c.F, LS_POINTER, { LS_POINTER, LS_I32 }, (void*) IA_l_vec<void*>, { a, k });
		// TODO tuple
		assert(0);
	}
}

}
