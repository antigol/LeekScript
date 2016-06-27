#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "../../compiler/value/Value.hpp"
#include "../Compiler.hpp"
class SemanticAnalyser;

namespace ls {

class Instruction {
public:

	Type type;

	virtual ~Instruction() = 0;

	virtual void print(std::ostream&) const = 0;

	virtual void analyse(SemanticAnalyser* analyser, const Type& type) = 0;

 	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const = 0;
};

}

#endif