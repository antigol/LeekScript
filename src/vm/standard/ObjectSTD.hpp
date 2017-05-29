#ifndef OBJECTSTD_HPP
#define OBJECTSTD_HPP

#include "../Module.hpp"

namespace ls {

class LSObject;

class ObjectSTD : public Module {
public:
	ObjectSTD();

	static LSObject readonly;
	static LSNumber* readonly_value;
};

}

#endif
