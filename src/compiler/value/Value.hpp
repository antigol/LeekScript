#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>

#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

class Value {
public:

	Type type;
	bool constant;
	bool parenthesis = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const = 0;

	virtual unsigned line() const = 0;

	virtual void analyse(SemanticAnalyser*, const Type&) = 0;

	virtual jit_value_t compile(Compiler&) const = 0;

	static std::string tabs(int indent);
};

}

#endif
