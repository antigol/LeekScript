#ifndef TYPE_LIST_HPP
#define TYPE_LIST_HPP

#include <set>
#include "Type.hpp"

namespace ls {

struct TypeList : public std::vector<Type> {

	TypeList& operator = (const Type& type);

	void add(TypeList& list);
	void add(const Type& type);
};

std::ostream& operator << (std::ostream&, const TypeList&);

}

#endif
