#include "LSClass.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"

using namespace std;

namespace ls {

LSValue* LSClass::class_class(new LSClass("Class"));

LSClass::LSClass() : LSClass("?") {}

LSClass::LSClass(string name) : name(name) {
	parent = nullptr;
	refs = 1;
	native = true;
}

LSClass::LSClass(Json&) {
	parent = nullptr;
	native = true;
	// TODO
}

LSClass::~LSClass() {
	for (auto s : static_fields) {
		if (s.second.value != nullptr) {
			delete s.second.value;
		}
	}
	for (auto m : default_methods) {
		delete m.second;
	}
}

void LSClass::addMethod(string& name, vector<Method> method) {
	methods.insert({name, method});
}

void LSClass::addStaticMethod(string& name, vector<StaticMethod> method) {
	static_methods.insert({name, method});

	// Add first implementation as default method
	LSFunction* fun = new LSFunction(method[0].addr);
	static_fields.insert({name, ModuleStaticField(name, Type::FUNCTION_P, fun)});
}

void LSClass::addField(std::string name, Type type) {
	fields.insert({name, type});
}

void LSClass::addStaticField(ModuleStaticField f) {
	static_fields.insert({f.name, f});
}

void LSClass::addStaticField(std::string name, Type type, LSValue* value) {
	static_fields.insert({name, ModuleStaticField(name, type, value)});
}

void LSClass::addOperator(std::string name, std::vector<Operator> impl) {
	operators.insert({name, impl});
}

Method* LSClass::getMethod(std::string& name, Type obj_type, vector<Type>& args) {

	try {
		vector<Method>& impl = methods.at(name);
		Method* best = nullptr;

		for (Method& m : impl) {

			if (m.obj_type.compatible(obj_type) and Type::list_compatible(m.type.arguments_types, args)) {
				if (best == nullptr or
					Type::list_more_specific(best->type.arguments_types, m.type.arguments_types) or
					Type::more_specific(best->obj_type, m.obj_type) /* old, new */) {
					best = &m;
				}
			}
		}
		return best;
	} catch (exception&) {
		return nullptr;
	}
}

StaticMethod* LSClass::getStaticMethod(std::string& name, vector<Type>& args) {

	try {
		vector<StaticMethod>& impl = static_methods.at(name);
		StaticMethod* best = nullptr;

		for (auto& m : impl) {
			if (Type::list_compatible(m.type.arguments_types, args)) {
				if (best == nullptr or Type::list_more_specific(best->type.arguments_types, m.type.arguments_types)) {
					best = &m;
				}
			}
		}
		return best;
	} catch (exception&) {
		return nullptr;
	}
}

LSFunction* LSClass::getDefaultMethod(string& name) {
	try {
		return default_methods.at(name);
	} catch (...) {
		try {
			vector<Method>& impl = methods.at(name);
			LSFunction* fun = new LSFunction(impl[0].addr);
			default_methods.insert({name, fun});
			return fun;
		} catch (...) {}
	}
	return nullptr;
}

LSClass::Operator* LSClass::getOperator(std::string& name, Type& obj_type, Type& operand_type) {

	//std::cout << "getOperator(" << name << ", " << obj_type << ", " << operand_type << ")" << std::endl;

	try {
		vector<Operator>& impl = operators.at(name);
		Operator* best = nullptr;

		for (Operator& m : impl) {
			if (m.object_type.compatible(obj_type) and m.operand_type.compatible(operand_type)) {
				if (best == nullptr or
					Type::more_specific(best->operand_type, m.operand_type) or
					Type::more_specific(best->object_type, m.object_type)) {
					best = &m;
				}
			}
		}
		return best;
	} catch (exception&) {
		return nullptr;
	}
}

bool LSClass::isTrue() const {
	return false;
}

bool LSClass::eq(const LSClass*) const {
	return false;
}

bool LSClass::lt(const LSClass*) const {
	return false;
}



LSValue* LSClass::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSClass::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSClass::attr(const LSValue* key) const {
	const LSString* str = dynamic_cast<const LSString*>(key);
	if (str) {
		if (str->compare("class") == 0) {
			return getClass();
		}
		if (str->compare("name") == 0) {
			return new LSString(name);
		}
		try {
			return static_fields.at(*str).value;
		} catch (exception&) {}
	}
	return LSNull::get();
}

LSValue** LSClass::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSClass::clone() const {
	return new LSClass(name);
}

std::ostream& LSClass::print(std::ostream& os) const {
	os << "<class " << name << ">";
	return os;
}

string LSClass::json() const {
	return "class";
}

LSValue* LSClass::getClass() const {
	return LSClass::class_class;
}

const BaseRawType* LSClass::getRawType() const {
	return RawType::CLASS;
}

}
