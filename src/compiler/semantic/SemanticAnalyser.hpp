#ifndef SEMANTICANALYSER_H_
#define SEMANTICANALYSER_H_

#include <stack>
#include <vector>
#include <map>

#include "../../vm/Type.hpp"
#include "SemanticException.hpp"

namespace ls {

class Program;
class Module;
class Method;
class Function;
class VariableValue;
class Context;
class Value;
class SemanticAnalyser;
class Token;
class VariableDeclaration;

enum class VarScope {
	INTERNAL, LOCAL, PARAMETER
};

class SemanticVar {
public:
	std::string name;
	VarScope scope;
	Type type;
	std::map<std::string, Type> attr_types;
	int index;
	Value* value;
	VariableDeclaration* vd;
	Function* function; // In which function the variable is declared

	SemanticVar(std::string name, VarScope scope, Type type, int index, Value* value,
		VariableDeclaration* vd, Function* function) :
		name(name), scope(scope), type(type), index(index), value(value), vd(vd), function(function) {}

	void must_be_pointer(SemanticAnalyser*);
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_program = false;

	std::vector<Module*> modules;

	std::map<std::string, SemanticVar*> internal_vars;
	std::vector<std::vector<std::map<std::string, SemanticVar*>>> variables;
	std::vector<std::map<std::string, SemanticVar*>> parameters;

//	std::vector<Function*> functions;
	std::stack<Function*> functions_stack;
	std::stack<int> loops;

	std::vector<SemanticException> errors;

	SemanticAnalyser(const std::vector<Module*>& modules);
	virtual ~SemanticAnalyser();

	void preanalyse(Program*);
	void analyse(Program*);

	void enter_function(Function*);
	void leave_function();
	void enter_block();
	void leave_block();
	void add_function(Function*);
	Function* current_function() const;

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	Module* module_by_name(const std::string& name) const;
	std::vector<Method> get_method(const std::string& name, const Type& return_type, const Type& this_type, const std::vector<Type>& args_types) const;

	SemanticVar* add_var(Token*, Type, Value*, VariableDeclaration*);
	SemanticVar* add_parameter(Token*, Type);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(std::string name);
	std::map<std::string, SemanticVar*>& get_local_vars();

	void add_error(SemanticException ex);

};

}

#endif
