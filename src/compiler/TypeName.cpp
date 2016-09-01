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

	for (const Type& t : { Type::BOOLEAN, Type::I32, Type::I64, Type::F32, Type::F64, Type::VAR }) {
		if (name->content == t.raw_type.name() && elements.empty()) {
			return t;
		}
	}

	if (name->content == Type::VEC.raw_type.name() && elements.size() == 1) {
		return Type(RawType::VEC, { elements[0]->getInternalType(analyser) });
	}
	if (name->content == Type::MAP.raw_type.name() && elements.size() == 2) {
		return Type(RawType::MAP, { elements[0]->getInternalType(analyser), elements[1]->getInternalType(analyser) });
	}
	if (name->content == Type::SET.raw_type.name() && elements.size() == 1) {
		return Type(RawType::SET, { elements[0]->getInternalType(analyser) });
	}
	if (name->content == Type::TUPLE.raw_type.name()) {
		Type type(RawType::TUPLE);
		for (size_t i = 0; i < elements.size(); ++i) {
			type.element_types.push_back(elements[i]->getInternalType(analyser));
		}
		return type;
	}
	if (name->content == Type::FUNCTION.raw_type.name()) {
		Type type(RawType::FUNCTION);
		for (size_t i = 0; i < arguments.size(); ++i) {
			type.arguments_types.push_back(arguments[i]->getInternalType(analyser));
		}
		type.setReturnType(returnType ? returnType->getInternalType(analyser) : Type::VOID);
		return type;
	}


	analyser->add_error({SemanticException::Type::UNKNOWN_TYPE, name->line, name->content});
	return Type::UNKNOWN;
}

}
