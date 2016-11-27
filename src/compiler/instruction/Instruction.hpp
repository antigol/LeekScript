#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"

namespace ls {

class Value;
class SemanticAnalyser;

class Instruction {
public:

	Type type = Type::VOID;

	virtual ~Instruction() = 0;

	virtual void print(std::ostream&, int indent, bool debug) const = 0;

	virtual void analyse(SemanticAnalyser* analyser, const Type& type) = 0;

 	virtual Compiler::value compile(Compiler&) const = 0;
};

}

#endif
