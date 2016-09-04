#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include "LeftValue.hpp"

namespace ls {

class ObjectAccess : public LeftValue {
public:

	Value* object;
	Token* field;
//	std::string object_class_name;
//	std::string class_name;
//	bool class_attr = false;
//	void* attr_addr;
//	void* access_function = nullptr;
//	Type field_type;

	ObjectAccess();
	virtual ~ObjectAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual void preanalyse(SemanticAnalyser* analyser) override;

	virtual jit_value_t compile(Compiler&) const override;

	virtual jit_value_t compile_l(Compiler&) const override;
};

}

#endif
