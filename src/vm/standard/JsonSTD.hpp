#ifndef JSON_STD_HPP
#define JSON_STD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class JsonSTD : public Module {
public:
	JsonSTD();

	static Compiler::value encode(Compiler&, std::vector<Compiler::value>);
	static Compiler::value decode(Compiler&, std::vector<Compiler::value>);
};

}

#endif