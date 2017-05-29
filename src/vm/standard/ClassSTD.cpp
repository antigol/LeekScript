#include "ClassSTD.hpp"
#include "../value/LSClass.hpp"

namespace ls {

ClassSTD::ClassSTD() : Module("Class") {

	LSClass::clazz = clazz;

	field("name", Type::STRING);


}

}
