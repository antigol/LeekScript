#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>

#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "Type.hpp"
#include "Program.hpp"

namespace ls {

//class StaticMethod {
//public:
//	Type type;
//	void* addr;
//	StaticMethod(Type return_type, std::initializer_list<Type> args, void* addr) {
//		this->addr = addr;
//		type = Type::FUNCTION;
//		type.set_return_type(return_type);
//		for (Type arg : args) {
//			type.add_argument_type(arg);
//		}
//	}
//};

class Method {
public:
	Type type;
	void* addr;
	Method() : addr(nullptr) {}
	Method(const Type &type, void *addr) : type(type), addr(addr) {}
	Method(const Type& obj_type, const Type& return_type, std::initializer_list<Type> args, void* addr) {
		this->addr = addr;
		type = Type::FUNCTION;
		type.set_return_type(return_type);
		type.add_argument_type(obj_type);
		for (Type arg : args) {
			type.add_argument_type(arg);
		}
	}
};

class ModuleMethod {
public:
	std::string name;
	std::vector<Method> impl;
	ModuleMethod(const std::string& name, const std::vector<Method>& impl)
	: name(name), impl(impl) {}
};

//class ModuleStaticMethod {
//public:
//	std::string name;
//	std::vector<StaticMethod> impl;
//	ModuleStaticMethod(std::string name, std::vector<StaticMethod> impl)
//	: name(name), impl(impl) {}
//};

//class ModuleStaticField {
//public:
//	std::string name;
//	Type type;
//	void* fun = nullptr;
//	LSValue* value = nullptr;

//	ModuleStaticField() {}
//	ModuleStaticField(const ModuleStaticField& f)
//	: name(f.name), type(f.type), fun(f.fun), value(f.value) {}
//	ModuleStaticField(std::string name, Type type, LSValue* value)
//	: name(name), type(type), value(value) {}
//	ModuleStaticField(std::string name, Type type, void* fun)
//	: name(name), type(type), fun(fun) {}
//};

//class ModuleField {
//public:
//	std::string name;
//	Type type;
//	ModuleField(std::string name, Type type) : name(name), type(type) {}
//};


class Module : public Type {
public:
	std::string name;
//	std::vector<ModuleField> fields;
	std::vector<ModuleMethod> methods;
//	std::vector<ModuleStaticField> static_fields;
//	std::vector<ModuleStaticMethod> static_methods;

	Module(const std::string& name);
	virtual ~Module();

	void method(const std::string& name, std::initializer_list<Method>);
	std::vector<Method> get_method_implementation(const std::string& name, const Type& return_type, const Type& this_type, const std::vector<Type> args_types) const;

//	void static_method(std::string name, std::initializer_list<StaticMethod>);
//	void static_method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);

//	void field(std::string name, Type type);
//	void static_field(std::string name, Type type, void* fun);

//	void include(SemanticAnalyser*, Program*);
	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
