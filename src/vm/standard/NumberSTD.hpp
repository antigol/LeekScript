#ifndef NUMBER_STD_HPP
#define NUMBER_STD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"
#include <math.h>

namespace ls {

class LSNumber;

class NumberSTD : public Module {
public:
	NumberSTD();
};

}

#endif
