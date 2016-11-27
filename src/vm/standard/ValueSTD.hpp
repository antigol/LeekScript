#ifndef VALUE_STD_HPP
#define VALUE_STD_HPP

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../VM.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class ValueSTD : public Module {
public:
	ValueSTD();

	/*
	 * Static fields
	 */
	static jit_value_t unknown(jit_function_t F);

	/*
	 * Operators
	 */
	static Compiler::value op_instanceof(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_gt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_xor(Compiler&, std::vector<Compiler::value>);

	/*
	 * Methods
	 */
	static Compiler::value to_string(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value to_json(Compiler& c, std::vector<Compiler::value> args);

	// Hidden functions
	static Compiler::value typeID(Compiler& c, std::vector<Compiler::value> args);
};

}

#endif
