#include "SystemSTD.hpp"
#include "../../compiler/jit/jit_general.hpp"
#include "../LSValue.hpp"
#include <chrono>

using namespace std;
using namespace ls;

static void System_print_ln() { cout << endl; }

class Print : public Method {
public:
	Print() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::UNKNOWN);
		generic_type.set_return_type(Type::VOID);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		jit_general::print(c.F, args[0], type.arguments_types[0]);
		jit_general::delete_temporary(c.F, args[0], type.arguments_types[0]);
		jit_general::call_native(c.F, LS_VOID, { }, (void*) System_print_ln, { });
		return nullptr;
	}
};

class String : public Method {
public:
	String() {
		generic_type = Type::FUNCTION;
		generic_type.add_argument_type(Type::UNKNOWN);
		generic_type.set_return_type(Type::VAR);
	}

	virtual jit_value_t compile(Compiler &c, const Type& type, const vector<jit_value_t>& args) override {
		jit_value_t res = jit_general::string(c.F, args[0], type.arguments_types[0]);
		jit_general::delete_temporary(c.F, args[0], type.arguments_types[0]);
		return res;
	}
};

static double System_time() {
	long t = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	return 1e-9 * t;
}

class Time : public Method {
public:
	Time() {
		generic_type = Type::FUNCTION;
		generic_type.set_return_type(Type::F64);
	}

	virtual jit_value_t compile(Compiler &c, const Type& , const vector<jit_value_t>& ) override {
		return jit_general::call_native(c.F, LS_F64, { }, (void*) System_time, { });
	}
};

SystemSTD::SystemSTD() : Module("ls")
{
	methods["print"] = new Print();
	methods["string"] = new String();
	methods["time"] = new Time();
}



