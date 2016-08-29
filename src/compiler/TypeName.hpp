#ifndef TYPENAME_H
#define TYPENAME_H

#include "semantic/SemanticAnalyser.hpp"
#include "lexical/Token.hpp"
#include "../vm/Type.hpp"
#include <string>
#include <vector>

namespace ls {

class TypeName
{
public:
	Token* name;
	std::vector<TypeName*> elements;

	TypeName();
	~TypeName();

	void print(std::ostream&) const;

	Type getInternalType(SemanticAnalyser*) const;
};

}

#endif // TYPENAME_H
