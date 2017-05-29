#ifndef VM_STANDARD_STRINGSTD_HPP_
#define VM_STANDARD_STRINGSTD_HPP_

#include "../Module.hpp"

namespace ls {

class LSString;
class LSNumber;

class StringSTD : public Module {
public:
	StringSTD();
	virtual ~StringSTD();

	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args);

	static LSString* add_int(LSString* s, int i);
	static LSString* add_real(LSString* s, double i);

	static LSString* replace(LSString*, LSString*, LSString*);
};

}

#endif
