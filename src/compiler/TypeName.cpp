#include "TypeName.hpp"

namespace ls {

TypeName::TypeName() : returnType(nullptr) {
}

TypeName::~TypeName() {
	for (TypeName* tn : elements)
		delete tn;
}

void TypeName::print(std::ostream& os) const
{
	os << getInternalType();
}

Type TypeName::getInternalType() const {

	for (const Type& t : { Type::BOOLEAN, Type::I32, Type::F64, Type::VAR }) {
		if (name->content == t.raw_type->name() && elements.empty()) {
			return t;
		}
	}

	if (name->content == Type::VEC.raw_type->name() && elements.size() == 1) {
		return Type(&RawType::VEC, { elements[0]->getInternalType() });
	}
	if (name->content == Type::MAP.raw_type->name() && elements.size() == 2) {
		return Type(&RawType::MAP, { elements[0]->getInternalType(), elements[1]->getInternalType() });
	}
	if (name->content == Type::SET.raw_type->name() && elements.size() == 1) {
		return Type(&RawType::SET, { elements[0]->getInternalType() });
	}
	if (name->content == Type::TUPLE.raw_type->name()) {
		Type type(&RawType::TUPLE);
		for (size_t i = 0; i < elements.size(); ++i) {
			type.elements_types.push_back(elements[i]->getInternalType());
		}
		return type;
	}
	if (name->content == Type::FUNCTION.raw_type->name()) {
		Type type(&RawType::FUNCTION);
		for (size_t i = 0; i < arguments.size(); ++i) {
			type.arguments_types.push_back(arguments[i]->getInternalType());
		}
		type.set_return_type(returnType ? returnType->getInternalType() : Type::VOID);
		return type;
	}

	return Type::UNKNOWN;
}

}
