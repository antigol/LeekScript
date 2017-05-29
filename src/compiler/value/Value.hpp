#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>
#include "../Compiler.hpp"
#include "../../vm/TypeList.hpp"
#include "../lexical/Location.hpp"

namespace ls {

class SemanticAnalyser;

class Value {
public:

	Type type;
	TypeList types;
	std::vector<Type> version;
	bool has_version = false;
	std::map<std::string, Type> attr_types;
	bool constant;
	bool parenthesis = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const = 0;
	std::string to_string() const;

	virtual Location location() const = 0;

	virtual bool will_take(SemanticAnalyser*, const std::vector<Type>& args_type, int level);
	virtual bool will_store(SemanticAnalyser*, const Type&);
	virtual bool must_be_pointer(SemanticAnalyser*);
	virtual void must_return(SemanticAnalyser*, const Type&);
	virtual void will_be_in_array(SemanticAnalyser*);
	virtual void set_version(std::vector<Type>);
	virtual Type version_type(std::vector<Type>) const;
	virtual void analyse(SemanticAnalyser*, const Type&) = 0;

	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const;
	virtual void compile_end(Compiler&) const {}

	virtual Value* clone() const = 0;

	static std::string tabs(int indent);
};

std::ostream& operator << (std::ostream& os, const Value* v);

}

#endif
