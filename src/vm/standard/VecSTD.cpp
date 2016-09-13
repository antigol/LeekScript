#include "VecSTD.hpp"
#include "../../compiler/jit/jit_vec.hpp"

using namespace std;
using namespace ls;

namespace vec {

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

}

VecSTD::VecSTD() : Module(RawType::VEC.clazz())
{
	methods["size"] = new vec::Size();
	methods["push"] = new vec::Push();
}

