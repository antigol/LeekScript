#ifndef LSCLASS_HPP_
#define LSCLASS_HPP_

#include <string>
#include <map>
#include "../LSValue.hpp"
#include "../Type.hpp"

namespace ls {

class Method;
class StaticMethod;
class ModuleField;
class ModuleStaticField;

class LSClass : public LSValue {
public:

	class Operator {
	public:
		Type object_type;
		Type operand_type;
		Type return_type;
		void* addr;
		bool native;
		Operator(Type object_type, Type operand, Type return_type, void* addr, bool native = false)
		: object_type(object_type), operand_type(operand), return_type(return_type), addr(addr), native(native)	{}
		static bool NATIVE;
	};

	LSClass* parent;
	std::string name;

	std::map<std::string, ModuleField> fields;
	std::map<std::string, std::vector<Method>> methods;
	std::map<std::string, std::vector<StaticMethod>> static_methods;
	std::map<std::string, ModuleStaticField> static_fields;
	std::map<std::string, std::vector<Operator>> operators;

	static LSValue* clazz;

	LSClass(std::string);

	virtual ~LSClass();

	void addMethod(std::string&, std::vector<Method>);
	void addField(std::string, Type, void* fun);
	void addStaticField(ModuleStaticField f);
	void addOperator(std::string name, std::vector<Operator>);

	Method* getMethod(std::string&, Type obj_type, std::vector<Type>&);
	void addStaticMethod(std::string& name, std::vector<StaticMethod> method);
	LSFunction<LSValue*>* getDefaultMethod(const std::string& name);
	StaticMethod* getStaticMethod(std::string&, std::vector<Type>&);
	Operator* getOperator(std::string& name, Type& object_type, Type& operand_type);

	bool to_bool() const override;
	virtual bool ls_not() const override;

	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	LSValue* attr(const std::string& key) const override;

	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;

	LSValue* getClass() const override;
};

}

#endif
