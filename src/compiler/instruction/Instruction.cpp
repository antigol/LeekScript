#include "../../compiler/instruction/Instruction.hpp"

namespace ls {

Instruction::~Instruction() {}

void Instruction::preanalyse(SemanticAnalyser* analyser)
{
	analyse(analyser, Type::UNKNOWN);
}

std::string Instruction::tabs(int indent) {
	return std::string(indent * 4, ' ');
}

}
