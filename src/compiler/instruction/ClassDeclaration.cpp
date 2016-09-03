#include "../../compiler/instruction/ClassDeclaration.hpp"

using namespace std;

namespace ls {

ClassDeclaration::ClassDeclaration() {}

ClassDeclaration::~ClassDeclaration() {}

void ClassDeclaration::print(ostream& os, int indent, bool debug) const {
	os << "class " << name << " {" << endl;
	for (VariableDeclaration* vd : fields) {
		vd->print(os, indent + 1, debug);
		os << endl;
	}
	os << "}";
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {
	for (VariableDeclaration* vd : fields) {
		vd->analyse(analyser, Type::UNKNOWN);
	}
	assert(type.is_complete());
}

jit_value_t ClassDeclaration::compile(Compiler&) const {
	return nullptr;
}

}
