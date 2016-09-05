#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Module.hpp"
#include "LSValue.hpp"
#include "value/LSVar.hpp"

using namespace std;

namespace ls {

Module::Module(const string& name) : name(name) {}

Module::~Module() {}

//void Module::include(SemanticAnalyser* analyser, Program* program) {
//	program->system_vars.insert({name, clazz});
//	analyser->add_var(new Token(name), Type::CLASS, nullptr, nullptr);
//}

//void Module::field(std::string name, Type type) {
//	fields.push_back(ModuleField(name, type));
//	clazz->addField(name, type);
//}

//void Module::static_field(std::string name, Type type, void* fun) {
//	static_fields.push_back(ModuleStaticField(name, type, fun));
//	clazz->addStaticField(ModuleStaticField(name, type, fun));
//}

void Module::method(const string& name, initializer_list<Method> impl) {
	methods.push_back(ModuleMethod(name, impl));
}

Method Module::get_method_implementation(const string& name, const Type& obj_type, const Type& return_type, const std::vector<Type> args) const
{
	Type proposal_type = Type::FUNCTION;
	proposal_type.set_return_type(return_type);
	proposal_type.set_argument_type(0, obj_type);
	for (const Type& a : args) proposal_type.add_argument_type(a);

	for (const ModuleMethod& method : methods) {
		if (method.name == name) {
			for (const Method& method_impl : method.impl) {
				Type completed_type;
				if (Type::intersection(proposal_type, method_impl.type, &completed_type)) {
					return Method(completed_type, method_impl.addr);
				}
			}
		}
	}
	return Method(Type::VOID, nullptr);
}

//void Module::static_method(string name, initializer_list<StaticMethod> impl) {
//	static_methods.push_back(ModuleStaticMethod(name, impl));
//	clazz->addStaticMethod(name, impl);
//}

//void Module::static_method(string name, Type return_type, initializer_list<Type> args, void* addr) {
//	static_methods.push_back(ModuleStaticMethod(name, {{return_type, args, addr}}));
//	clazz->addStaticMethod(name, {{return_type, args, addr}});
//}

void Module::generate_doc(std::ostream& os, std::string translation_file) {

	ifstream f;
	f.open(translation_file);
	stringstream j;
	j << f.rdbuf();
	std::string str = j.str();
	f.close();

	// Erase tabs
	str.erase(std::remove(str.begin(), str.end(), '	'), str.end());

	// Parse json
	Json translation = Json::parse(str);

	map<std::string, Json> translation_map;

	for (Json::iterator it = translation.begin(); it != translation.end(); ++it) {
		translation_map.insert({it.key(), it.value()});
	}

	os << "\"" << name << "\":{";

//	os << "\"attributes\":{";
//	for (unsigned e = 0; e < static_fields.size(); ++e) {

//		ModuleStaticField& a = static_fields[e];

//		std::string desc = (translation_map.find(a.name) != translation_map.end()) ?
//				translation_map[a.name] : "";

//		if (e > 0) os << ",";
//		os << "\"" << a.name << "\":{\"type\":";
//		a.type.toJson(os);
//		//os << ",\"value\":\"" << a.value << "\"";
//		os << ",\"desc\":\"" << desc << "\"";
//		os << "}";
//	}

	os << "},\"methods\":{";
	for (unsigned e = 0; e < methods.size(); ++e) {
		ModuleMethod& m = methods[e];

		if (e > 0) os << ",";
		os << "\"" << m.name << "\":{\"type\":";
		m.impl[0].type.toJson(os);

		if (translation_map.find(m.name) != translation_map.end()) {
			Json json = translation_map[m.name];
			std::string desc = json["desc"];
			std::string return_desc = json["return"];

			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
	}

//	os << "},\"static_methods\":{";
//	for (unsigned e = 0; e < static_methods.size(); ++e) {
//		ModuleStaticMethod& m = static_methods[e];

//		if (e > 0) os << ",";
//		os << "\"" << m.name << "\":{\"type\":";
//		m.impl[0].type.toJson(os);

//		if (translation_map.find(m.name) != translation_map.end()) {
//			Json json = translation_map[m.name];
//			std::string desc = json["desc"];
//			std::string return_desc = json["return"];

//			os << ",\"desc\":\"" << desc << "\"";
//			os << ",\"return\":\"" << return_desc << "\"";
//		}
//		os << "}";
//	}
	os << "}}";
}

}
