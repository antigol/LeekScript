#ifndef VM_STANDARD_ARRAYSTD_HPP_
#define VM_STANDARD_ARRAYSTD_HPP_

#include "../value/LSFunction.hpp"
#include "../Module.hpp"

namespace ls {

class ArraySTD : public Module {
public:
	ArraySTD();

	static Compiler::value lt(Compiler&, std::vector<Compiler::value>);

	static Compiler::value size(Compiler&, std::vector<Compiler::value>);
};

}

#endif
