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
class Block;
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
	VarScope scope_type;
	Type type;
	int index;
	Value* scope; // In which the variable has been declared
	VariableDeclaration* vd;
	Function* function; // In which the variable has been declared

	SemanticVar(std::string name, VarScope scope_type, Type type, int index, Value* scope,
		VariableDeclaration* vd, Function* function) :
		name(name), scope_type(scope_type), type(type), index(index), scope(scope), vd(vd), function(function) {}
};

class SemanticAnalyser {
public:

	Program* program;
	bool in_program = false;
#if DEBUG >= 1
	int in_phase = 0; // only for debug 1=analyse 2=reanalyse 3=finalize
#endif

	std::vector<Module*> modules;

	std::map<std::string, SemanticVar*> internal_vars;
	std::vector<std::vector<std::map<std::string, SemanticVar*>>> variables;
	std::vector<std::map<std::string, SemanticVar*>> parameters;

	std::stack<Value*> block_stack;
	std::stack<Function*> functions_stack;
	std::stack<int> loops;

	std::vector<SemanticException> errors;

	SemanticAnalyser(const std::vector<Module*>& modules);
	virtual ~SemanticAnalyser();

	void analyse(Program*);
	void finalize(Program*);

	void enter_function(Function*);
	void leave_function();
	void enter_block(Value* block);
	void leave_block();
	void add_function(Function*);
	Function* current_function() const;
	Value* current_block() const;

	void enter_loop();
	void leave_loop();
	bool in_loop(int deepness) const;

	SemanticVar* add_var(Token*, Type, Value* scope, VariableDeclaration*);
	SemanticVar* add_parameter(Token*, Type, Value* scope);

	SemanticVar* get_var(Token* name);
	SemanticVar* get_var_direct(std::string name);
	std::map<std::string, SemanticVar*>& get_local_vars();

	std::vector<Method*> get_method(const std::string& module_name, const std::string& name, const Type& method_type, Type* result_type) const;

	void add_error(SemanticException ex);

};

}

#endif
