#include "../../compiler/instruction/ClassDeclaration.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

using namespace std;

namespace ls {

ClassDeclaration::ClassDeclaration(std::shared_ptr<Token> token) : token(token) {
	name = token->content;
	var = nullptr;
	ls_class = new LSClass(name);
}

ClassDeclaration::~ClassDeclaration() {
	for (const auto& vd : fields) {
		delete vd;
	}
	delete ls_class;
}

void ClassDeclaration::print(ostream& os, int indent, bool debug) const {
	os << "class " << name << " {" << endl;
	for (VariableDeclaration* vd : fields) {
		vd->print(os, indent + 1, debug);
		os << endl;
	}
	os << "}";
}

Location ClassDeclaration::location() const {
	return token->location;
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	var = analyser->add_var(token.get(), Type::CLASS, nullptr, nullptr);

	for (auto vd : fields) {
		vd->analyse(analyser, Type::UNKNOWN);
	}
}

Compiler::value ClassDeclaration::compile(Compiler& c) const {

	auto clazz = c.new_pointer(ls_class);

	c.add_var(name, clazz.v, Type::CLASS, true);

	return clazz;
}

Instruction* ClassDeclaration::clone() const {
	auto cd = new ClassDeclaration(token);
	cd->name = name;
	for (const auto& f : fields) {
		cd->fields.push_back((VariableDeclaration*) f->clone());
	}
	return cd;
}

}
