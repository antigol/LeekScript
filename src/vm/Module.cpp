#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Module.hpp"
#include "LSValue.hpp"
#include "value/LSVar.hpp"

using namespace std;
using namespace ls;

Module::Module(const string& name) : name(name) {}

Module::~Module()
{
	for (const pair<string, Method*>& x : methods) {
		delete x.second;
	}
}

Method* Module::get_method_implementation(const string& name, const Type& req_type, Type* result_type) const
{
	auto it = methods.find(name);

	if (it != methods.end()) {
		Method* m = it->second;
		if (Type::intersection(req_type, m->generic_type, result_type)) {
			return m;
		}
	}
	return nullptr;
}
