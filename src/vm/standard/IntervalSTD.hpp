#ifndef SRC_VM_STANDARD_INTERVALSTD_HPP_
#define SRC_VM_STANDARD_INTERVALSTD_HPP_

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../VM.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class IntervalSTD : public Module {
public:
	IntervalSTD();
	virtual ~IntervalSTD();
};

}

#endif
