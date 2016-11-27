#ifndef VM_STANDARD_STRINGSTD_HPP_
#define VM_STANDARD_STRINGSTD_HPP_

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class LSString;
class LSNumber;

class StringSTD : public Module {
public:
	StringSTD();
	virtual ~StringSTD();

	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args);
};

}

#endif
