#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>

#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"
#include "../../vm/value/LSString.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ObjectAccess : public LeftValue {
public:

	Value* object;
	Token* field;
	std::string object_class_name;
	std::string class_name;
	bool class_attr = false;
	void* attr_addr;
	void* access_function = nullptr;
	LSString* field_string = nullptr;
	Type field_type;

	ObjectAccess();
	virtual ~ObjectAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Compiler::value compile_l(Compiler&) const override;
};

}

#endif
