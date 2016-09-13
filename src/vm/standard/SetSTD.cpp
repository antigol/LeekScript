#include "SetSTD.hpp"
#include "../../compiler/jit/jit_general.hpp"

using namespace std;
using namespace ls;

class Size : public Method {
public:
	Size() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::SET);
		generic_type.set_return_type(Type::I32);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		Type elem_type = type.arguments_types[0].elements_types[0];
		jit_value_t res = jit_set::size(c.F, args[0]);
		jit_set::delete_temporary(c.F, elem_type, args[0]);
		return res;
	}
};

class Insert : public Method {
public:
	Insert() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type(&RawType::SET, { Type::UNKNOWN.placeholder(1) }));
		generic_type.add_argument_type(Type::UNKNOWN.placeholder(1));
		generic_type.set_return_type(Type::BOOLEAN);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		assert(type.is_pure());
		Type elem_type = type.arguments_types[1];
		jit_value_t res = jit_set::insert_move_inc(c.F, elem_type, args[0], args[1]);
		jit_set::delete_temporary(c.F, elem_type, args[0]);
		return res;
	}
};

SetSTD::SetSTD() : Module(RawType::SET.clazz())
{
	methods["size"] = new Size();
	methods["insert"] = new Insert();
}
