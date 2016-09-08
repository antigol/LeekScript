#include <sstream>
#include <chrono>
#include <cassert>

#include "VM.hpp"

#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticException.hpp"
#include "LSValue.hpp"
#include "value/LSVec.hpp"
#include "value/LSVar.hpp"
#include "Program.hpp"
#include "../compiler/Compiler.hpp"

#include "standard/VecSTD.hpp"

using namespace std;

namespace ls {

VM::VM() {
	add_module(new VecSTD());
}

VM::~VM() {
	for (Module* m : modules) {
		delete m;
	}
}

unsigned int VM::operations = 0;
const bool VM::enable_operations = true;
const unsigned int VM::operation_limit = 2000000000;

map<string, jit_value_t> internals;

void VM::add_module(Module* m) {
	modules.push_back(m);
}

#if DEBUG > 1
extern std::map<LSValue*, LSValue*> objs;
#endif

string VM::execute(const std::string code, std::string ctx, ExecMode mode) {

//	Type t1 = Type(&RawType::TUPLE, { Type::UNKNOWN.place_holder(1),                                    Type(&RawType::VEC, { Type::UNKNOWN.place_holder(1) }), Type::UNKNOWN.place_holder(2), Type::UNKNOWN.place_holder(2) });
//	Type t2 = Type(&RawType::TUPLE, { Type(&RawType::UNKNOWN, { Type::I32, Type::F64, Type::LSVALUE }), Type(&RawType::VEC, { Type::UNKNOWN.place_holder(1) }), Type::UNKNOWN.place_holder(1), Type::VAR });

//	Type t1 = Type(&RawType::TUPLE, { Type::UNKNOWN.place_holder(1),      Type(&RawType::MAP, { Type({ Type::I32, Type::F64, Type::LSVALUE }), Type::UNKNOWN.place_holder(1) }),
//									  Type::UNKNOWN.place_holder(2), Type::UNKNOWN.place_holder(2) });
//	Type t2 = Type(&RawType::TUPLE, { Type({ Type::I32, Type::LSVALUE }), Type(&RawType::MAP, { Type::I32,                                     Type::UNKNOWN.place_holder(1) }),
//									  Type::UNKNOWN.place_holder(1), Type::UNKNOWN });

//	Type t1 = Type(&RawType::TUPLE, { Type(&RawType::VEC, {Type::UNKNOWN.place_holder(1)}),                  Type::UNKNOWN.place_holder(1)              });
//	Type t2 = Type(&RawType::TUPLE, { Type(&RawType::VEC, {Type(&RawType::VEC, {Type::I32})}),               Type(&RawType::VEC, { Type::UNKNOWN })     });

//	Type t1 = Type(&RawType::TUPLE, { Type(&RawType::VEC, {Type::UNKNOWN.placeholder(1)}),                  Type::UNKNOWN.placeholder(1)              });
//	Type t2 = Type(&RawType::TUPLE, { Type(&RawType::VEC, {Type(&RawType::VEC, {Type::I32})}),               Type(&RawType::VEC, { Type::UNREACHABLE })     });

//	Type t1 = Type::FUNCTION;
//	t1.add_argument_type(Type(&RawType::VEC, {Type::UNKNOWN.place_holder(1)}));
//	t1.add_argument_type(Type(&RawType::VEC, {Type::UNKNOWN.place_holder(1)}));
//	t1.add_argument_type(Type::UNKNOWN.place_holder(1));

//	Type t2 = Type::FUNCTION;
//	t2.add_argument_type(Type(&RawType::VEC, {Type(&RawType::VEC, { Type::UNKNOWN })}));
//	t2.add_argument_type(Type(&RawType::VEC, {Type(&RawType::VEC, {Type::I32})}));
//	t2.add_argument_type(Type(&RawType::VEC, { Type::UNKNOWN }));

//	Type t1 = Type(&RawType::TUPLE, { Type(&RawType::VEC, { Type::UNKNOWN }).placeholder(1),                  Type::UNKNOWN.placeholder(1),  Type::UNKNOWN });
//	Type t2 = Type(&RawType::TUPLE, { Type(&RawType::VEC, { Type::I32 }).placeholder(1),                     Type::UNKNOWN ,                 Type::UNKNOWN.placeholder(1)});

//	Type re;
//	cout << Type::intersection(t1, t2, &re) << endl;
//	cout << re << endl << endl;

	// Reset
	LSValue::obj_count = 0;
	LSValue::obj_deleted = 0;
	#if DEBUG > 1
		objs.clear();
	#endif

	Program* program = new Program(code);

	// Compile
	double compile_time = program->compile(*this, ctx, mode);
	if (compile_time < 0.0) {
		delete program;
		return "{}";
	}

	// Execute
	VM::operations = 0;

	auto exe_start = chrono::high_resolution_clock::now();
	string result = program->execute();
	auto exe_end = chrono::high_resolution_clock::now();

	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();

	double exe_time_ms = (((double) exe_time_ns / 1000) / 1000);

	/*
	 * Return results
	 */
	if (mode == ExecMode::COMMAND_JSON || mode == ExecMode::TOP_LEVEL) {

		string ctx = "{";

	//		unsigned i = 0;
	/*
		for (auto g : globals) {
			if (globals_ref[g.first]) continue;
			LSValue* v = res_vec->operator[] (i + 1);
			ctx += "\"" + g.first + "\":" + v->to_json();
			if (i < globals.size() - 1) ctx += ",";
			i++;
		}
		*/
		ctx += "}";

		if (mode == ExecMode::TOP_LEVEL) {
			cout << result << endl;
			cout << "(" << VM::operations << " ops, " << compile_time << " ms + " << exe_time_ms << " ms)" << endl;
			result = ctx;
		} else {
			cout << "{\"success\":true,\"ops\":" << VM::operations << ",\"time\":" << exe_time_ns << ",\"ctx\":" << ctx << ",\"res\":\""
					<< result << "\"}" << endl;
			result = ctx;
		}

	} else if (mode == ExecMode::FILE_JSON) {

//		LSVec<LSValue*>* res_vec = (LSVec<LSValue*>*) res;

//		ostringstream oss;
//		res_vec->operator[] (0)->print(oss);
//		result = oss.str();

//		LSValue::delete_temporary(res);

		string ctx;

		cout << "{\"success\":true,\"ops\":" << VM::operations << ",\"time\":" << exe_time_ns
			 << ",\"ctx\":" << ctx << ",\"res\":\"" << result << "\"}" << endl;


	} else if (mode == ExecMode::NORMAL) {

		cout << result << endl;
		cout << "(" << VM::operations << " ops, " << compile_time << "ms + " << exe_time_ms << " ms)" << endl;

	} else if (mode == ExecMode::TEST) {
	} else if (mode == ExecMode::TEST_OPS) {
		result = to_string(VM::operations);
	}

	// Cleaning
	delete program;

	#if DEBUG > 0
		if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
			cout << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << endl;
			#if DEBUG > 1
				for (auto o : objs) {
					o.second->print(cout);
					cout << " (" << o.second->refs << " refs)" << endl;
				}
			#endif
		} else {
			cout << ls::LSValue::obj_count << " objects created" << endl;
		}
	#endif

	return result;
}

LSValue* VM_convert_i32(int32_t n) {
	return new LSVar(n);
}
LSValue* VM_convert_i64(int64_t n) {
	return new LSVar(n);
}
LSValue* VM_convert_bool(int32_t n) {
	return new LSVar((bool) n);
}
LSValue* VM_convert_f32(float n) {
	return new LSVar(n);
}
LSValue* VM_convert_f64(double n) {
	return new LSVar(n);
}

jit_value_t VM::value_to_lsvalue(jit_function_t F, jit_value_t v, Type type)
{
	if (type == Type::I32)     return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_convert_i32, { v });
	if (type == Type::I64)     return Compiler::call_native(F, LS_POINTER, { LS_I64 }, (void*) VM_convert_i64, { v });
	if (type == Type::F32)     return Compiler::call_native(F, LS_POINTER, { LS_F32 }, (void*) VM_convert_f32, { v });
	if (type == Type::F64)     return Compiler::call_native(F, LS_POINTER, { LS_F64 }, (void*) VM_convert_f64, { v });
	if (type == Type::BOOLEAN) return Compiler::call_native(F, LS_POINTER, { LS_BOOLEAN }, (void*) VM_convert_bool, { v });
	assert(0);
	return nullptr;
}

int32_t VM_get_refs(LSValue* val) {
	return val ? val->refs : 0;
}

jit_value_t VM::get_refs(jit_function_t F, jit_value_t ptr) {
	return Compiler::call_native(F, LS_I32, { LS_POINTER }, (void*) VM_get_refs, { ptr });
}

void VM_inc_refs(LSValue* val) {
	val->refs++;
}

void VM::inc_refs(jit_function_t F, jit_value_t ptr) {
	Compiler::call_native(F, jit_type_void, { LS_POINTER }, (void*) VM_inc_refs, { ptr });
}

void VM_inc_refs_if_not_temp(LSValue* val) {
	if (val && val->refs != 0) {
		val->refs++;
	}
}

void VM::inc_refs_if_not_temp(jit_function_t F, jit_value_t ptr) {
	Compiler::call_native(F, jit_type_void, { LS_POINTER }, (void*) VM_inc_refs_if_not_temp, { ptr });
}

void VM_dec_refs(LSValue* val) {
	if (val) val->refs--;
}

void VM::dec_refs(jit_function_t F, jit_value_t ptr) {
	Compiler::call_native(F, jit_type_void, { LS_POINTER }, (void*) VM_dec_refs, { ptr });
}

void VM::delete_ref(jit_function_t F, jit_value_t ptr) {
	Compiler::call_native(F, jit_type_void, { LS_POINTER }, (void*) LSValue::delete_ref, { ptr });
}

void VM::delete_temporary(jit_function_t F, jit_value_t ptr) {
	Compiler::call_native(F, jit_type_void, { LS_POINTER }, (void*) LSValue::delete_temporary, { ptr });
}


void VM_operation_exception() {
	throw vm_operation_exception();
}

void VM::inc_ops(jit_function_t F, int add) {

	if (!enable_operations) return;

	// Variable counter pointer
	jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, LS_POINTER, (long int) &VM::operations);

	// Increment counter
	jit_value_t jit_ops = jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);
	jit_insn_store_relative(F, jit_ops_ptr, 0, jit_insn_add(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, add)));

	// Compare to the limit
	jit_value_t compare = jit_insn_gt(F, jit_ops, jit_value_create_nint_constant(F, jit_type_uint, VM::operation_limit));
	jit_label_t label_end = jit_label_undefined;
	jit_insn_branch_if_not(F, compare, &label_end);

	// If greater than the limit, throw exception
//	jit_type_t args[1] = {JIT_INTEGER};
//	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
//	jit_insn_call_native(F, "throw_exception", (void*) VM_operation_exception, sig, &jit_ops, 1, JIT_CALL_NOTHROW);
	jit_insn_throw(F, jit_value_create_nint_constant(F, jit_type_int, 12));

	// End
	jit_insn_label(F, &label_end);
}

void VM_print_int(int val) {
//	cout << val << endl;
	cout << "Execution ended, too much operations: " << VM::operations << " (" << val << ")" << endl;
}

void VM::print_int(jit_function_t F, jit_value_t val) {
	Compiler::call_native(F, LS_VOID, { LS_I32 }, (void*) VM_print_int, { val });
}

jit_value_t VM::create_bool(jit_function_t F, bool value)
{
	return create_i32(F, value);
}

jit_value_t VM::create_i32(jit_function_t F, int32_t value)
{
	return jit_value_create_nint_constant(F, LS_I32, value);
}

jit_value_t VM::create_i64(jit_function_t F, int64_t value)
{
	return jit_value_create_long_constant(F, LS_I64, value);
}

jit_value_t VM::create_f32(jit_function_t F, double value)
{
	return jit_value_create_float32_constant(F, LS_F32, value);
}

jit_value_t VM::create_f64(jit_function_t F, double value)
{
	return jit_value_create_float64_constant(F, LS_F64, value);
}

jit_value_t VM::create_ptr(jit_function_t F, void* value)
{
	jit_constant_t constant = { LS_POINTER, { value } };
	return jit_value_create_constant(F, &constant);
}

jit_value_t VM::create_null(jit_function_t F) {
	return create_ptr(F, nullptr);
}

LSValue* VM_create_bool(int value) {
	return new LSVar((bool) value);
}

jit_value_t VM::create_lsbool(jit_function_t F, bool value)
{
	return Compiler::call_native(F, LS_POINTER, { LS_BOOLEAN }, (void*) VM_create_bool, { create_bool(F, value) });
}

LSValue* VM_create_real(double value) {
	return new LSVar(value);
}

jit_value_t VM::create_lsreal(jit_function_t F, double value)
{
	return Compiler::call_native(F, LS_POINTER, { LS_F64 }, (void*) VM_create_real, { create_f64(F, value) });
}

jit_value_t VM::create_default(jit_function_t F, const Type& type)
{
	if (type.raw_type->nature() == Nature::LSVALUE) return create_null(F);
	if (type == Type::VOID) return nullptr;
	if (type == Type::BOOLEAN) return create_bool(F, false);
	if (type == Type::I32) return create_i32(F, 0);
	if (type == Type::I64) return create_i64(F, 0);
	if (type == Type::F32) return create_f32(F, 0.0);
	if (type == Type::F64) return create_f64(F, 0.0);
	if (type.raw_type == &RawType::FUNCTION) return create_ptr(F, nullptr);

	assert(0);
}

template <typename T>
LSVec<T>* VM_create_vec(int32_t cap) {
	LSVec<T>* vec = new LSVec<T>();
	vec->reserve(cap);
	return vec;
}

jit_value_t VM::create_vec(jit_function_t F, const Type& element_type, int cap) {
	jit_value_t s = create_i32(F, cap);

	if (element_type == Type::BOOLEAN) return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_create_vec<int32_t>, { s });
	if (element_type == Type::I32)     return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_create_vec<int32_t>, { s });
	if (element_type == Type::F64)     return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_create_vec<double>, { s });
	if (element_type.raw_type->nature() == Nature::LSVALUE)
									   return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_create_vec<LSValue*>, { s });
	if (element_type.raw_type == &RawType::FUNCTION)
									   return Compiler::call_native(F, LS_POINTER, { LS_I32 }, (void*) VM_create_vec<void*>, { s });
	assert(0);
}

void VM_push_vec_lsptr(LSVec<LSValue*>* vec, LSValue* value) {
	vec->push_back(LSValue::move_inc(value));
}

template <typename T>
void VM_push_vec(LSVec<T>* vec, T value) {
	vec->push_back(value);
}

void VM::push_move_inc_vec(jit_function_t F, const Type& element_type, jit_value_t vec, jit_value_t value) {
	/* Because of the move, there is no need to call delete_temporary on the pushed value.
	 * If value points to a temporary variable his ownership will be transfer to the vec.
	 */

	if (element_type == Type::I32) {
		Compiler::call_native(F, LS_VOID, { LS_POINTER, element_type.jit_type() }, (void*) VM_push_vec<int32_t>, { vec, value });
	} else 	if (element_type == Type::F64) {
		Compiler::call_native(F, LS_VOID, { LS_POINTER, element_type.jit_type() }, (void*) VM_push_vec<double>, { vec, value });
	} else 	if (element_type.raw_type->nature() == Nature::LSVALUE) {
		Compiler::call_native(F, LS_VOID, { LS_POINTER, element_type.jit_type() }, (void*) VM_push_vec_lsptr, { vec, value });
	} else if (element_type.raw_type == &RawType::FUNCTION) {
		Compiler::call_native(F, LS_VOID, { LS_POINTER, element_type.jit_type() }, (void*) VM_push_vec<void*>, { vec, value });
	} else {
		assert(0);
	}

}

jit_value_t VM::move_obj(jit_function_t F, jit_value_t ptr) {
	return Compiler::call_native(F, LS_POINTER, { LS_POINTER }, (void*) LSValue::move, { ptr });
}

jit_value_t VM::move_inc_obj(jit_function_t F, jit_value_t ptr) {
	return Compiler::call_native(F, LS_POINTER, { LS_POINTER }, (void*) LSValue::move_inc, { ptr });
}

LSValue* VM_clone(LSValue* val) {
	return val->clone();
}

jit_value_t VM::clone_obj(jit_function_t F, jit_value_t ptr) {
	return Compiler::call_native(F, LS_POINTER, { LS_POINTER }, (void*) VM_clone, { ptr });
}

bool VM_is_true(LSValue* val) {
	if (val == nullptr) return false;
	return val->isTrue();
}

jit_value_t VM::is_true(jit_function_t F, jit_value_t ptr) {
	return Compiler::call_native(F, jit_type_sys_bool, { LS_POINTER }, (void*) VM_is_true, { ptr });
}

}
