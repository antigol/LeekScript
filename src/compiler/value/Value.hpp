#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>

#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"

namespace ls {

class SemanticAnalyser;

class Value {
public:

	Type type;
	std::map<std::string, Type> attr_types;
	bool constant;
	bool parenthesis = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const = 0;
	std::string to_string() const;

	virtual unsigned line() const = 0;

	virtual bool will_take(SemanticAnalyser*, const std::vector<Type>& args_type);
	virtual bool will_take_element(SemanticAnalyser*, const Type);
	virtual bool will_store(SemanticAnalyser*, const Type&);
	virtual bool must_be_pointer(SemanticAnalyser*);
	virtual void must_return(SemanticAnalyser*, const Type&);
	virtual void analyse(SemanticAnalyser*, const Type&) = 0;

	virtual Compiler::value compile(Compiler&) const = 0;

	static std::string tabs(int indent);
};

std::ostream& operator << (std::ostream& os, const Value* v);

}

#endif
