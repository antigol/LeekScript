#include "SemanticAnalyser.hpp"
#include "../../compiler/instruction/ExpressionInstruction.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Context.hpp"
#include "SemanticError.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Module.hpp"

#include <functional>

using namespace std;

namespace ls {

SemanticAnalyser::SemanticAnalyser() {
	program = nullptr;
//	loops.push(0);
//	variables.push_back(vector<map<std::string, SemanticVar*>> {});
//	functions_stack.push(nullptr); // The first function is the main function of the program
//	parameters.push_back(map<std::string, SemanticVar*> {});
}

SemanticAnalyser::~SemanticAnalyser() {}

void SemanticVar::must_be_pointer(SemanticAnalyser* analyser) {
	if (value != nullptr) {
		value->must_be_pointer(analyser);
		this->type.nature = Nature::POINTER;
	}
}

void SemanticAnalyser::analyse(Program* program, Context*) {

	this->program = program;

	enter_function(program->main);

	// Add context variables
	/*
	for (auto var : context->vars) {
		add_var(new Token(var.first), Type(var.second->getRawType(), Nature::POINTER), nullptr, nullptr);
	}
	*/
	program->analyse(this);
	program->functions = functions;
}

void SemanticAnalyser::enter_function(Function* f) {

	// Create function scope
	variables.push_back(vector<map<std::string, std::shared_ptr<SemanticVar>>> {});
	// First function block
	variables.back().push_back(map<std::string, std::shared_ptr<SemanticVar>> {});
	// Parameters
	parameters.push_back(map<std::string, std::shared_ptr<SemanticVar>> {});

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
	variables.back().push_back(map<std::string, std::shared_ptr<SemanticVar>> {});
}

void SemanticAnalyser::leave_block() {
	variables.back().pop_back();
}

Function* SemanticAnalyser::current_function() const {
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

std::shared_ptr<SemanticVar> SemanticAnalyser::add_parameter(Token* v, Type type) {

	auto arg = std::make_shared<SemanticVar>(v->content, VarScope::PARAMETER, type, parameters.back().size(), nullptr, nullptr, current_function());
	parameters.back().insert({v->content, arg});
	return arg;
}

std::shared_ptr<SemanticVar> SemanticAnalyser::get_var(Token* v) {

	// Search in interval variables : global for the program
	try {
		return vm->internal_vars.at(v->content);
	} catch (exception& e) {}

	// Search recursively in the functions
	int f = functions_stack.size() - 1;
	while (f >= 0) {
		// Search in the function parameters
		try {
			return parameters.at(f).at(v->content);
		} catch (exception& e) {}

		// Search in the local variables of the function
		int b = variables.at(f).size() - 1;
		while (b >= 0) {
			try {
				return variables.at(f).at(b).at(v->content);
			} catch (exception& e) {}
			b--;
		}
		f--;
	}
	add_error({SemanticError::Type::UNDEFINED_VARIABLE, v->location, v->location, {v->content}});
	return nullptr;
}

std::shared_ptr<SemanticVar> SemanticAnalyser::add_var(Token* v, Type type, Value* value, VariableDeclaration* vd) {

	if (vm->internal_vars.find(v->content) != vm->internal_vars.end()) {
		add_error({SemanticError::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	if (variables.back().back().find(v->content) != variables.back().back().end()) {
		add_error({SemanticError::Type::VARIABLE_ALREADY_DEFINED, v->location, v->location, {v->content}});
		return nullptr;
	}
	variables.back().back().insert(pair<string, std::shared_ptr<SemanticVar>>(
		v->content,
		std::make_shared<SemanticVar>(v->content, VarScope::LOCAL, type, 0, value, vd, current_function())
	));
	return variables.back().back().at(v->content);
}

void SemanticAnalyser::add_function(Function* l) {
	functions.push_back(l);
}

map<string, std::shared_ptr<SemanticVar>>& SemanticAnalyser::get_local_vars() {
	return variables.back().back();
}

void SemanticAnalyser::add_error(SemanticError ex) {
	ex.underline_code = program->underline_code(ex.location, ex.focus);
	ex.file = program->file_name;
	errors.push_back(ex);
}

} // end of namespace ls
