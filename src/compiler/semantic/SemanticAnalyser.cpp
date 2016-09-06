#include "../../compiler/semantic/SemanticAnalyser.hpp"

#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../../vm/standard/NullSTD.hpp"
#include "../../vm/standard/NumberSTD.hpp"
#include "../../vm/standard/BooleanSTD.hpp"
#include "../../vm/standard/StringSTD.hpp"
#include "../../vm/standard/VecSTD.hpp"
#include "../../vm/standard/MapSTD.hpp"
#include "../../vm/standard/SetSTD.hpp"
#include "../../vm/standard/ObjectSTD.hpp"
#include "../../vm/standard/SystemSTD.hpp"
#include "../../vm/standard/FunctionSTD.hpp"
#include "../../vm/standard/ClassSTD.hpp"
#include "../../vm/value/LSVar.hpp"
#include "SemanticException.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

SemanticAnalyser::SemanticAnalyser(const std::vector<Module*>& modules) {
	program = nullptr;
	in_program = false;
	this->modules = modules;
//	loops.push(0);
//	variables.push_back(vector<map<std::string, SemanticVar*>> {});
//	functions_stack.push(nullptr); // The first function is the main function of the program
//	parameters.push_back(map<std::string, SemanticVar*> {});
}

SemanticAnalyser::~SemanticAnalyser() {}

void SemanticAnalyser::preanalyse(Program* program)
{
	// Gives to each element a type
	// but this type is not necessarly complete
	in_program = true;
	program->main->preanalyse(this);
}

void SemanticAnalyser::analyse(Program* program)
{
	// Fix the uncomplete types
	program->main->analyse(this, Type::UNKNOWN);
}

void SemanticAnalyser::enter_function(Function* f) {

	// Create function scope
	variables.push_back(vector<map<std::string, SemanticVar*>> {});
	// First function block
	variables.back().push_back(map<std::string, SemanticVar*> {});
	// Parameters
	parameters.push_back(map<std::string, SemanticVar*> {});

	loops.push(0);
	functions_stack.push(f);
}

void SemanticAnalyser::leave_function() {

	variables.pop_back();
	parameters.pop_back();
	functions_stack.pop();
	loops.pop();
}

void SemanticAnalyser::enter_block() {
	variables.back().push_back(map<std::string, SemanticVar*> {});
}

void SemanticAnalyser::leave_block() {
	variables.back().pop_back();
}

Function* SemanticAnalyser::current_function() const {
	if (functions_stack.empty()) {
		return nullptr;
	}
	return functions_stack.top();
}

void SemanticAnalyser::enter_loop() {
	loops.top()++;
}

void SemanticAnalyser::leave_loop() {
	loops.top()--;
}

bool SemanticAnalyser::in_loop(int deepness) const {
	return loops.top() >= deepness;
}

Module* SemanticAnalyser::module_by_name(const string& name) const
{
	for (size_t i = 0; i < modules.size(); ++i) {
		if (modules[i]->name == name) return modules[i];
	}
	return nullptr;
}

vector<Method> SemanticAnalyser::get_method(const string& name, const Type& return_type, const Type& this_type, const std::vector<Type>& args_types) const
{
	string clazz = this_type.get_raw_type()->clazz();
	if (clazz.empty()) {
		vector<Method> methods;
		for (Module* module : modules) {
			vector<Method> x = module->get_method_implementation(name, return_type, this_type, args_types);
			methods.insert(methods.end(), x.begin(), x.end());
		}
		return methods;
	} else {
		Module* module = module_by_name(clazz);
		return module->get_method_implementation(name, return_type, this_type, args_types);
	}
}

SemanticVar* SemanticAnalyser::add_parameter(Token* v, Type type) {

	SemanticVar* arg = new SemanticVar(v->content, VarScope::PARAMETER, type, parameters.back().size(), nullptr, nullptr, current_function());
	parameters.back().insert(pair<string, SemanticVar*>(v->content, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* v) {

	// Search in internal variables : global for the program
	try {
		return internal_vars.at(v->content);
	} catch (exception&) {}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		try {
			return parameters[f].at(v->content);
		} catch (exception&) {}

		// Search in the local variables of the function
		int b = variables[f].size() - 1;
		while (b >= 0) {
			try {
				return variables[f][b].at(v->content);
			} catch (exception&) {}
			b--;
		}
		f--;
	}
	add_error({ SemanticException::Type::UNDEFINED_VARIABLE, v->line, v->content });
	return nullptr;
}

SemanticVar* SemanticAnalyser::get_var_direct(std::string name) {
	try {
		if (variables.size() > 0) {
			return variables.back().back().at(name);
		}
	} catch (exception& e) {}
	return nullptr;
}

SemanticVar* SemanticAnalyser::add_var(Token* v, Type type, Value* value, VariableDeclaration* vd) {

	// Internal variable, before execution
	if (!in_program) {
		internal_vars.insert(pair<string, SemanticVar*>(
			v->content,
			new SemanticVar(v->content, VarScope::INTERNAL, type, 0, value, vd, current_function())
		));
		return internal_vars.at(v->content);
	}

	if (variables.back().back().find(v->content) != variables.back().back().end()) {
		add_error({SemanticException::Type::VARIABLE_ALREADY_DEFINED, v->line, v->content});
	}
	variables.back().back().insert(pair<string, SemanticVar*>(
		v->content,
		new SemanticVar(v->content, VarScope::LOCAL, type, 0, value, vd, current_function())
	));
	return variables.back().back().at(v->content);
}

//void SemanticAnalyser::add_function(Function* l) {
//	functions.push_back(l);
//}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	return variables.back().back();
}

void SemanticAnalyser::add_error(SemanticException ex) {
	errors.push_back(ex);
}

} // end of namespace ls
