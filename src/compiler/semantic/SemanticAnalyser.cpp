#include "../../compiler/semantic/SemanticAnalyser.hpp"

#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "../../vm/standard/VecSTD.hpp"
#include "../../vm/standard/SystemSTD.hpp"
#include "../../vm/value/LSVar.hpp"
#include "SemanticException.hpp"
#include "../instruction/VariableDeclaration.hpp"

#include <cassert>

using namespace std;

namespace ls {

SemanticAnalyser::SemanticAnalyser(const std::vector<Module*>& modules) {
	program = nullptr;
	in_program = false;
	this->modules = modules;
}

SemanticAnalyser::~SemanticAnalyser() {}

void SemanticAnalyser::analyse(Program* program)
{
	// Gives to each element a type
	// but this type is not necessarly complete
	in_program = true;

#if DEBUG >= 1
	in_phase = 1;
#endif
	program->main->analyse(this);
#if DEBUG >= 1
	in_phase = 0;
#endif
	if (!errors.empty()) return;

#if DEBUG >= 1
	Value::reanalyse_deepness = 0;
	Value::reanalyse_maximum_deepness = 0;
	in_phase = 2;
#endif

	program->main->reanalyse(this, Type::UNKNOWN);

#if DEBUG >= 1
	in_phase = 0;
	cout << "maximum reanalyse deepness " << Value::reanalyse_maximum_deepness << endl;
#endif
}

void SemanticAnalyser::finalize(Program* program)
{
	// Fix the uncomplete types
#if DEBUG >= 1
	in_phase = 3;
#endif

	program->main->finalize(this, Type::UNKNOWN); // TODO do not recreate variables enter_block and so on

#if DEBUG >= 1
	in_phase = 0;
#endif
}

void SemanticAnalyser::enter_function(Function* f) {

	assert(in_phase == 1);
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

	assert(in_phase == 1);
	variables.pop_back();
	parameters.pop_back();
	functions_stack.pop();
	loops.pop();
}

void SemanticAnalyser::enter_block(Value* block) {
	assert(in_phase == 1);
	variables.back().push_back(map<std::string, SemanticVar*> {});
	block_stack.push(block);
}

void SemanticAnalyser::leave_block() {
	assert(in_phase == 1);
	variables.back().pop_back();
	block_stack.pop();
}

Function* SemanticAnalyser::current_function() const {
	assert(in_phase == 1);
	if (functions_stack.empty()) {
		assert(0);
		return nullptr;
	}
	return functions_stack.top();
}

Value*SemanticAnalyser::current_block() const
{
	assert(in_phase == 1);
	if (block_stack.empty()) {
		assert(0);
		return nullptr;
	}
	return block_stack.top();
}

void SemanticAnalyser::enter_loop() {
	assert(in_phase == 1);
	loops.top()++;
}

void SemanticAnalyser::leave_loop() {
	assert(in_phase == 1);
	loops.top()--;
}

bool SemanticAnalyser::in_loop(int deepness) const {
	assert(in_phase == 1);
	return loops.top() >= deepness;
}

SemanticVar* SemanticAnalyser::add_var(const string& name, const Type& type, Value* scope) {
	assert(in_phase == 1);

	// Internal variable, before execution
	if (!in_program) {
		SemanticVar* var = new SemanticVar(name, VarScope::INTERNAL, type, 0, scope);
		internal_vars.insert(pair<string, SemanticVar*>(name, var));
		return var;
	}

//	if (variables.back().back().find(v->content) != variables.back().back().end()) {
//		add_error({ SemanticException::VARIABLE_ALREADY_DEFINED, v->line, v->content });
//	}

	SemanticVar* var = new SemanticVar(name, VarScope::LOCAL, type, 0, scope);

	variables.back().back()[name] = var;
	return var;
}

SemanticVar* SemanticAnalyser::add_parameter(Token* v, Type type, Value* scope) {
	assert(in_phase == 1);

	SemanticVar* arg = new SemanticVar(v->content, VarScope::PARAMETER, type, parameters.back().size(), scope);
	parameters.back().insert(pair<string, SemanticVar*>(v->content, arg));
	return arg;
}

SemanticVar* SemanticAnalyser::get_var(Token* v) {
	assert(in_phase == 1);

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
	assert(in_phase == 1);
	try {
		if (variables.size() > 0) {
			return variables.back().back().at(name);
		}
	} catch (exception&) {}
	return nullptr;
}

map<string, SemanticVar*>& SemanticAnalyser::get_local_vars() {
	assert(in_phase == 1);
	return variables.back().back();
}

vector<Method*> SemanticAnalyser::get_method(const string& module_name, const string& name, const Type& method_type, Type* result_type) const
{
	vector<Method*> methods;

	if (module_name.empty()) {
		for (Module* module : modules) {
			Type tmp;
			Method* x = module->get_method_implementation(name, method_type, &tmp);
			if (x) {
				if (methods.empty()) {
					*result_type = tmp;
				} else {
					*result_type = Type::union_of(*result_type, tmp);
				}
				methods.push_back(x);
			}
		}
	} else {
		Module* module = nullptr;
		for (size_t i = 0; i < modules.size(); ++i) {
			if (modules[i]->name == module_name) module = modules[i];
		}
		if (module)	{
			Method* x = module->get_method_implementation(name, method_type, result_type);
			if (x) methods.push_back(x);
		} else {
			assert(0);
		}
	}
	return methods;
}

void SemanticAnalyser::add_error(SemanticException ex) {
	errors.push_back(ex);
}

} // end of namespace ls
