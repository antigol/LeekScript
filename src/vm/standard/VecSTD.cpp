#include <algorithm>
#include "VecSTD.hpp"
#include "../value/LSVec.hpp"
#include "../../compiler/jit/jit_vec.hpp"

using namespace std;
using namespace ls;

class Size : public Method {
public:
	Size() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::VEC);
		generic_type.set_return_type(Type::I32);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		Type elem_type = type.arguments_types[0].elements_types[0];
		jit_value_t res = jit_vec::size(c.F, elem_type, args[0]);
		jit_vec::delete_temporary(c.F, elem_type, args[0]);
		return res;
	}
};

class Push : public Method {
public:
	Push() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type(&RawType::VEC, { Type::UNKNOWN.placeholder(1) }));
		generic_type.add_argument_type(Type::UNKNOWN.placeholder(1));
		generic_type.set_return_type(Type(&RawType::VEC, { Type::UNKNOWN.placeholder(1) }));
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		assert(type.is_pure());
		jit_vec::push_move_inc(c.F, type.arguments_types[0].elements_types[0], args[0], args[1]);
		return args[0];
	}
};

VecSTD::VecSTD() : Module(RawType::VEC.clazz())
{
	methods["size"] = new Size();
	methods["push"] = new Push();


	/*
	Type vec_ls1 = Type(&RawType::VEC, { Type::LSVALUE.placeholder(1) });
	Type vec_fn1 = Type(&RawType::VEC, { Type::FUNCTION.placeholder(1) });
	Type vec_ls = Type(&RawType::VEC, { Type::LSVALUE });
	Type vec_fn = Type(&RawType::VEC, { Type::FUNCTION });
	Type vec_bool = Type(&RawType::VEC, { Type::BOOLEAN });
	Type vec_i32 = Type(&RawType::VEC, { Type::I32 });
	Type vec_f64 = Type(&RawType::VEC, { Type::F64 });

	method("push", {
		{vec_bool, vec_bool, { Type::BOOLEAN },                 (void*) LSVec<int32_t>::ls_push},
		{vec_i32,  vec_i32,  { Type::I32 },                     (void*) LSVec<int32_t>::ls_push},
		{vec_f64,  vec_f64,  { Type::F64 },                     (void*) LSVec<double>::ls_push},
		{vec_ls1,  vec_ls1,  { Type::LSVALUE.placeholder(1) },  (void*) LSVec<LSValue*>::ls_push},
		{vec_fn1,  vec_fn1,  { Type::FUNCTION.placeholder(1) }, (void*) LSVec<void*>::ls_push},
	});


	method("size", {
		{vec_bool, Type::I32, { }, (void*) LSVec<int32_t>::ls_size},
		{vec_i32,  Type::I32, { }, (void*) LSVec<int32_t>::ls_size},
		{vec_f64,  Type::I32, { }, (void*) LSVec<double>::ls_size},
		{vec_ls,   Type::I32, { }, (void*) LSVec<LSValue*>::ls_size},
		{vec_fn,   Type::I32, { }, (void*) LSVec<void*>::ls_size},
	});
	*/
}
