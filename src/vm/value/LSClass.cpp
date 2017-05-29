#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"

using namespace std;

namespace ls {

LSValue* LSClass::clazz;

LSClass::LSClass(string name) : LSValue(CLASS), name(name) {
	parent = nullptr;
	refs = 1;
	native = true;
}

LSClass::~LSClass() {
	for (auto s : static_fields) {
		if (s.second.value != nullptr) {
			delete s.second.value;
		}
	}
}

void LSClass::addMethod(string& name, vector<Method> method) {
	methods.insert({name, method});
}

void LSClass::addStaticMethod(string& name, vector<StaticMethod> method) {
	static_methods.insert({name, method});

	// Add first implementation as default method
	auto fun = new LSFunction<LSValue*>(method[0].addr);
	fun->refs = 1;
	fun->native = true;
	Type type = method[0].type;
	static_fields.insert({name, ModuleStaticField(name, type, fun)});
}

void LSClass::addField(std::string name, Type type, void* fun) {
	fields.insert({name, ModuleField(name, type, fun)});
}

void LSClass::addStaticField(ModuleStaticField f) {
	static_fields.insert({f.name, f});
}

void LSClass::addOperator(std::string name, std::vector<Operator> impl) {
	operators.insert({name, impl});
}

Method* LSClass::getMethod(std::string& name, Type obj_type, vector<Type>& args) {
	try {
		vector<Method>& impl = methods.at(name);
		Method* best = nullptr;

		for (Method& m : impl) {

			if (m.obj_type.may_be_compatible(obj_type) and Type::list_may_be_compatible(m.type.arguments_types, args)) {
				if (best == nullptr or
					Type::more_specific(best->type.getReturnType(), m.type.getReturnType()) or
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
			if (Type::list_may_be_compatible(m.type.arguments_types, args)) {
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

LSFunction<LSValue*>* LSClass::getDefaultMethod(const string& name) {
	try {
		auto f = static_fields.at(name);
		f.value->refs++;
		return (LSFunction<LSValue*>*) f.value;
	} catch (...) {
		return nullptr;
	}
}

LSClass::Operator* LSClass::getOperator(std::string& name, Type& obj_type, Type& operand_type) {

	//std::cout << "getOperator(" << name << ", " << obj_type << ", " << operand_type << ")" << std::endl;
	if (name == "is not") name = "!=";
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

bool LSClass::to_bool() const {
	return true;
}

bool LSClass::ls_not() const {
	return false;
}

bool LSClass::eq(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return clazz->name == this->name;
	}
	return false;
}

bool LSClass::lt(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return this->name < clazz->name;
	}
	return LSValue::lt(v);
}

LSValue* LSClass::attr(const std::string& key) const {
	if (key == "name") {
		return new LSString(name);
	}
	try {
		return static_fields.at(key).value;
	} catch (exception&) {
		return LSValue::attr(key);
	}
}

std::ostream& LSClass::dump(std::ostream& os) const {
	os << "<class " << name << ">";
	return os;
}

string LSClass::json() const {
	return "\"<class " + name + ">\"";
}

LSValue* LSClass::getClass() const {
	return LSClass::clazz;
}

}
