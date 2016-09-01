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
#include "Program.hpp"

using namespace std;

namespace ls {

VM::VM() {}

VM::~VM() {}

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
	LSValue* res = program->execute();
	auto exe_end = chrono::high_resolution_clock::now();

	long exe_time_ns = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();

	double exe_time_ms = (((double) exe_time_ns / 1000) / 1000);

	/*
	 * Return results
	 */
	string result;

	if (mode == ExecMode::COMMAND_JSON || mode == ExecMode::TOP_LEVEL) {

		ostringstream oss;
		res->print(oss);
		result = oss.str();

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
		LSValue::delete_temporary(res);

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

		LSVec<LSValue*>* res_vec = (LSVec<LSValue*>*) res;

		ostringstream oss;
		res_vec->operator[] (0)->print(oss);
		result = oss.str();

		LSValue::delete_temporary(res);

		string ctx;

		cout << "{\"success\":true,\"ops\":" << VM::operations << ",\"time\":" << exe_time_ns
			 << ",\"ctx\":" << ctx << ",\"res\":\"" << result << "\"}" << endl;


	} else if (mode == ExecMode::NORMAL) {

		ostringstream oss;
		res->print(oss);
		LSValue::delete_temporary(res);
		string res_string = oss.str();

		string ctx;

		cout << res_string << endl;
		cout << "(" << VM::operations << " ops, " << compile_time << "ms + " << exe_time_ms << " ms)" << endl;

		result = ctx;

	} else if (mode == ExecMode::TEST) {

		ostringstream oss;
		res->print(oss);
		result = oss.str();

		LSValue::delete_temporary(res);

	} else if (mode == ExecMode::TEST_OPS) {

		LSValue::delete_temporary(res);
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

jit_type_t VM::get_jit_type(const Type& type) {
	if (type == Type::VOID) {
		return LS_VOID;
	}
	if (type.nature == Nature::LSVALUE || type.raw_type == RawType::FUNCTION) {
		return LS_POINTER;
	}
	if (type.raw_type == RawType::I32) {
		return LS_I32;
	}
	if (type.raw_type == RawType::BOOLEAN) {
		return LS_BOOLEAN;
	}
	if (type.raw_type == RawType::I64) {
		return LS_I64;
	}
	if (type.raw_type == RawType::F64) {
		return LS_F64;
	}
	assert(0);
	return LS_I32;
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

void* get_conv_fun(Type type) {
	if (type.raw_type == RawType::I32) {
		return (void*) &VM_convert_i32;
	}
	if (type.raw_type == RawType::I64) {
		return (void*) &VM_convert_i64;
	}
	if (type.raw_type == RawType::F32) {
		return (void*) &VM_convert_f32;
	}
	if (type.raw_type == RawType::F64) {
		return (void*) &VM_convert_f64;
	}
	if (type.raw_type == RawType::BOOLEAN) {
		return (void*) &VM_convert_bool;
	}
	assert(0);
	return nullptr;
}

jit_value_t VM::value_to_lsvalue(jit_function_t F, jit_value_t v, Type type) {

	void* fun = get_conv_fun(type);

	if (jit_type_get_kind(jit_value_get_type(v)) == JIT_TYPE_FLOAT64) {
		fun = (void*) &VM_convert_f64;
	}

	jit_type_t args_types[1] = { get_jit_type(type) };

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args_types, 1, 0);
	return jit_insn_call_native(F, "convert", (void*) fun, sig, &v, 1, JIT_CALL_NOTHROW);
}

int VM_get_refs(LSValue* val) {
	return val->refs;
}

jit_value_t VM::get_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "get_refs", (void*) VM_get_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs(LSValue* val) {
	val->refs++;
}

void VM::inc_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "inc_refs", (void*) VM_inc_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_inc_refs_if_not_temp(LSValue* val) {
	if (val->refs != 0) {
		val->refs++;
	}
}

void VM::inc_refs_if_not_temp(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "inc_refs_not_temp", (void*) VM_inc_refs_if_not_temp, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM_dec_refs(LSValue* val) {
	val->refs--;
}

void VM::dec_refs(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "dec_refs", (void*) VM_dec_refs, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::delete_ref(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "delete", (void*) &LSValue::delete_ref, sig, &obj, 1, JIT_CALL_NOTHROW);
}

void VM::delete_temporary(jit_function_t F, jit_value_t obj) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "delete_temporary", (void*) &LSValue::delete_temporary, sig, &obj, 1, JIT_CALL_NOTHROW);
}


void VM_operation_exception() {
	throw vm_operation_exception();
}

void VM::inc_ops(jit_function_t F, int add) {

	if (not enable_operations) return;

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
	jit_type_t args[1] = {LS_I32};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
	jit_insn_call_native(F, "print_int", (void*) VM_print_int, sig, &val, 1, JIT_CALL_NOTHROW);
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

LSValue* VM_create_null() {
	return new LSVar();
}

jit_value_t VM::create_null(jit_function_t F) {
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, {}, 0, 0);
	return jit_insn_call_native(F, "create_null", (void*) VM_create_null, sig, {}, 0, JIT_CALL_NOTHROW);
}

LSValue* VM_create_bool(int value) {
	return new LSVar((bool) value);
}

jit_value_t VM::create_lsbool(jit_function_t F, bool value)
{
	jit_type_t args[1] = { LS_BOOLEAN };
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	jit_value_t s = create_bool(F, value);

	return jit_insn_call_native(F, "create_bool", (void*) VM_create_bool, sig, &s, 1, JIT_CALL_NOTHROW);
}

LSVec<void*>* VM_create_vec_voidptr(int cap) {
	LSVec<void*>* vec = new LSVec<void*>();
	vec->reserve(cap);
	return vec;
}

LSVec<LSValue*>* VM_create_vec_lsptr(int cap) {
	LSVec<LSValue*>* vec = new LSVec<LSValue*>();
	vec->reserve(cap);
	return vec;
}

LSVec<int>* VM_create_vec_i32(int cap) {
	LSVec<int>* vec = new LSVec<int32_t>();
	vec->reserve(cap);
	return vec;
}

LSVec<double>* VM_create_vec_f64(int cap) {
	LSVec<double>* vec = new LSVec<double>();
	vec->reserve(cap);
	return vec;
}

jit_value_t VM::create_vec(jit_function_t F, const Type& element_type, int cap) {
	jit_type_t args[1] = {LS_I32};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	jit_value_t s = create_i32(F, cap);

	if (element_type == Type::I32) {
		return jit_insn_call_native(F, "create_vec", (void*) VM_create_vec_i32, sig, &s, 1, JIT_CALL_NOTHROW);
	}
	if (element_type == Type::F64) {
		return jit_insn_call_native(F, "create_vec", (void*) VM_create_vec_f64, sig, &s, 1, JIT_CALL_NOTHROW);
	}
	if (element_type.nature == Nature::LSVALUE) {
		return jit_insn_call_native(F, "create_vec", (void*) VM_create_vec_lsptr, sig, &s, 1, JIT_CALL_NOTHROW);
	}
	if (element_type.raw_type == RawType::FUNCTION) {
		return jit_insn_call_native(F, "create_vec", (void*) VM_create_vec_voidptr, sig, &s, 1, JIT_CALL_NOTHROW);
	}
}

void VM_push_vec_voidptr(LSVec<void*>* vec, void* value) {
	vec->push_move(value);
}

void VM_push_vec_lsptr(LSVec<LSValue*>* vec, LSValue* value) {
	vec->push_move(value);
}

void VM_push_vec_i32(LSVec<int32_t>* vec, int32_t value) {
	vec->push_back(value);
}

void VM_push_vec_f64(LSVec<double>* vec, double value) {
	vec->push_back(value);
}

void VM::push_move_vec(jit_function_t F, const Type& element_type, jit_value_t vec, jit_value_t value) {
	/* Because of the move, there is no need to call delete_temporary on the pushed value.
	 * If value points to a temporary variable his ownership will be transfer to the vec.
	 */
	jit_type_t args[2] = {LS_POINTER, get_jit_type(element_type)};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 2, 0);
	jit_value_t args_v[] = {vec, value};

	if (element_type == Type::I32) {
		jit_insn_call_native(F, "push_vec", (void*) VM_push_vec_i32, sig, args_v, 2, JIT_CALL_NOTHROW);
	}
	if (element_type == Type::F64) {
		jit_insn_call_native(F, "push_vec", (void*) VM_push_vec_f64, sig, args_v, 2, JIT_CALL_NOTHROW);
	}
	if (element_type.nature == Nature::LSVALUE) {
		jit_insn_call_native(F, "push_vec", (void*) VM_push_vec_lsptr, sig, args_v, 2, JIT_CALL_NOTHROW);
	}
	if (element_type.raw_type == RawType::FUNCTION) {
		jit_insn_call_native(F, "push_vec", (void*) VM_push_vec_voidptr, sig, args_v, 2, JIT_CALL_NOTHROW);
	}
}

LSValue* VM_move(LSValue* val) {
	return val->move();
}

jit_value_t VM::move_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "move", (void*) VM_move, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

LSValue* VM_move_inc(LSValue* val) {
	return val->move_inc();
}

jit_value_t VM::move_inc_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "move_inc", (void*) VM_move_inc, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

LSValue* VM_clone(LSValue* val) {
	return val->clone();
}

jit_value_t VM::clone_obj(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_POINTER, args, 1, 0);
	return jit_insn_call_native(F, "clone", (void*) VM_clone, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

bool VM_is_true(LSValue* val) {
	return val->isTrue();
}

jit_value_t VM::is_true(jit_function_t F, jit_value_t ptr) {
	jit_type_t args[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args, 1, 0);
	return jit_insn_call_native(F, "is_true", (void*) VM_is_true, sig, &ptr, 1, JIT_CALL_NOTHROW);
}

}
