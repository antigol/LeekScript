#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../compiler/value/Function.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"

namespace ls {

class LSValue;

class Program {
private:

	std::string code; // The program code
	jit_function_t function;
	void* closure;

	void compile_main(Context&);
	void compile_jit(Compiler&, Context&, bool);

public:

	Function* main;
	std::vector<Function*> functions;
	std::map<std::string, LSValue*> system_vars;


	Program(const std::string& code);
	virtual ~Program();

	/*
	 * Compile the program with a VM and a context (json)
	 */
	VM::Result compile(VM& vm, const std::string& context);

	/*
	 * Execute the program and get a LSValue* result
	 */
	std::string execute();

	void print(std::ostream& os, bool debug = false) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
