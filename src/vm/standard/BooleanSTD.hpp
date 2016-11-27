#ifndef BOOLEAN_STD_HPP
#define BOOLEAN_STD_HPP

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../VM.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class BooleanSTD : public Module {
public:
	BooleanSTD();

	static LSString* add(int boolean, LSString* string);
	static LSString* add_tmp(int boolean, LSString* string);

	static int compare_ptr_ptr(LSBoolean* a, LSBoolean* b);
	static Compiler::value compare_val_val(Compiler&, std::vector<Compiler::value>);
};

}

#endif
