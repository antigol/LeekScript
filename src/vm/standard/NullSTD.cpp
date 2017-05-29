#include "NullSTD.hpp"
#include "../value/LSNull.hpp"

namespace ls {

NullSTD::NullSTD() : Module("Null") {
	LSNull::clazz = clazz;
}

}
