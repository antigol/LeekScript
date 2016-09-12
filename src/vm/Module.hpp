#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include <map>

#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "Type.hpp"
#include "Program.hpp"

namespace ls {

class Method {
public:
	Type generic_type;

	virtual jit_value_t compile(Compiler& c, const Type& type, const std::vector<jit_value_t>& args) = 0;
};

class Module : public Type {
public:
	std::string name;
	std::map<std::string, Method*> methods;

	Module(const std::string& name);
	virtual ~Module();

	Method* get_method_implementation(const std::string& name, const Type& req_type, Type* result_type) const;
};

}

#endif
