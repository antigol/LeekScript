#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>

#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "Type.hpp"
#include "Program.hpp"
#include "value/LSClass.hpp"

namespace ls {

class LSClass;

class StaticMethod {
public:
	Type type;
	void* addr;
	bool native;
	StaticMethod(Type return_type, std::initializer_list<Type> args, void* addr, bool native = false) {
		this->addr = addr;
		type = {RawType::FUNCTION, Nature::POINTER};
		type.setReturnType(return_type);
		for (Type arg : args) {
			type.addArgumentType(arg);
		}
		this->native = native;
	}
};

class Method {
public:
	Type type;
	void* addr;
	Type obj_type;
	bool native;
	Method(Type obj_type, Type return_type, std::initializer_list<Type> args, void* addr, bool native = false) {
		this->addr = addr;
		this->obj_type = obj_type;
		type = {RawType::FUNCTION, Nature::POINTER};
		type.setReturnType(return_type);
		for (Type arg : args) {
			type.addArgumentType(arg);
		}
		this->native = native;
	}
	static bool NATIVE;
};

class ModuleMethod {
public:
	std::string name;
	std::vector<Method> impl;
	ModuleMethod(std::string name, std::vector<Method> impl)
	: name(name), impl(impl) {}
};

class ModuleStaticMethod {
public:
	std::string name;
	std::vector<StaticMethod> impl;
	ModuleStaticMethod(std::string name, std::vector<StaticMethod> impl)
	: name(name), impl(impl) {}
};

class ModuleStaticField {
public:
	std::string name;
	Type type;
	void* fun = nullptr;
	LSValue* value = nullptr;

	ModuleStaticField() {}
	ModuleStaticField(const ModuleStaticField& f)
	: name(f.name), type(f.type), fun(f.fun), value(f.value) {}
	ModuleStaticField(std::string name, Type type, LSValue* value)
	: name(name), type(type), value(value) {}
	ModuleStaticField(std::string name, Type type, void* fun)
	: name(name), type(type), fun(fun) {}
};

class ModuleField {
public:
	std::string name;
	Type type;
	ModuleField(std::string name, Type type) : name(name), type(type) {}
};


class Module : public Type {
public:

	std::string name;
	LSClass* clazz;
	std::vector<ModuleField> fields;
	std::vector<ModuleMethod> methods;
	std::vector<ModuleStaticField> static_fields;
	std::vector<ModuleStaticMethod> static_methods;

	Module(std::string name);
	virtual ~Module();

	void operator_(std::string name, std::initializer_list<LSClass::Operator>);

	void method(std::string name, std::initializer_list<Method>);
	void method(std::string name, Type obj_type, Type return_type, std::initializer_list<Type> args, void* addr);

	void static_method(std::string name, std::initializer_list<StaticMethod>);
	void static_method(std::string name, Type return_type, std::initializer_list<Type> args, void* addr);

	void field(std::string name, Type type);
	void static_field(std::string name, Type type, void* fun);

	void include(SemanticAnalyser*, Program*);
	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
