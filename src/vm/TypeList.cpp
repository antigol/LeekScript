#include "TypeList.hpp"
#include <sstream>

namespace ls {

TypeList& TypeList::operator = (const Type& type) {
	clear();
	add(type);
	return *this;
}

void TypeList::add(TypeList& list) {
	for (auto& type : list) {
		add(type);
	}
}

void TypeList::add(const Type& type) {
	for (const auto& t : *this) {
		if (type == t)
			return;
	}
	push_back(type);
}

std::ostream& operator << (std::ostream& os, const TypeList& type) {
	//if (type.size() == 1) {
		// os << *type.begin();
	// } else {
		os << "{";
		for (auto i = type.begin(); i != type.end(); ++i) {
			if (i != type.begin()) os << ", ";
			os << *i;
		}
		os << "}";
	// }
	return os;
}

}
