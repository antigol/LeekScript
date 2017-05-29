#ifndef BOOLEAN_STD_HPP
#define BOOLEAN_STD_HPP

#include "../Module.hpp"

namespace ls {

class BooleanSTD : public Module {
public:
	BooleanSTD();

	static LSString* add(int boolean, LSString* string);
	static LSString* add_tmp(int boolean, LSString* string);
	static Compiler::value add_bool(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value sub_bool(Compiler& c, std::vector<Compiler::value> args);

	static Compiler::value mul_bool(Compiler& c, std::vector<Compiler::value> args);

	static int compare_ptr_ptr(LSBoolean* a, LSBoolean* b);
	static Compiler::value compare_val_val(Compiler&, std::vector<Compiler::value>);
};

}

#endif
