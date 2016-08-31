#include "TypeName.hpp"

namespace ls {

TypeName::TypeName() : returnType(nullptr) {
}

TypeName::~TypeName() {
	for (TypeName* tn : elements)
		delete tn;
}

void TypeName::print(std::ostream& os) const {
	os << name->content;
	if (!elements.empty()) {
		os << "<";
		for (size_t i = 0; i < elements.size(); ++i) {
			elements[i]->print(os);

			if (i < elements.size() - 1) os << ", ";
		}
		os << ">";
	}
	if (!arguments.empty()) {
		os << "(";
		for (size_t i = 0; i < arguments.size(); ++i) {
			arguments[i]->print(os);

			if (i < arguments.size() - 1) os << ", ";
		}
		os << ")";
	}
	if (returnType) {
		os << "->";
		returnType->print(os);
	}
}

Type TypeName::getInternalType(SemanticAnalyser* analyser) const {
	if (name->content == "bool" && elements.empty()) {
		return Type::BOOLEAN;
	}
	if (name->content == "int" && elements.empty()) {
		return Type::INTEGER;
	}
	if (name->content == "real" && elements.empty()) {
		return Type::FLOAT;
	}
	if (name->content == "string" && elements.empty()) {
		return Type::STRING;
	}
	if (name->content == "array" && elements.size() == 1) {
		return Type(RawType::ARRAY, Nature::LSVALUE, elements[0]->getInternalType(analyser));
	}
	if (name->content == "map" && elements.size() == 2) {
		return Type(RawType::MAP, Nature::LSVALUE, { elements[0]->getInternalType(analyser), elements[1]->getInternalType(analyser) });
	}
	if (name->content == "set" && elements.size() == 1) {
		return Type(RawType::SET, Nature::LSVALUE, elements[0]->getInternalType(analyser));
	}

	analyser->add_error({SemanticException::Type::UNKNOWN_TYPE, name->line, name->content});
	return Type::UNKNOWN;
}

}
