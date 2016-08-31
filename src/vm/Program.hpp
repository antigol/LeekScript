#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../compiler/value/Function.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"

namespace ls {

class LSValue;

class Program {
private:

	std::string code; // The program code
	void* closure;

	void compile_main(Context&);
	void compile_jit(Compiler&, Context&, bool);

public:

	Function* main;
	std::vector<Function*> functions;
	std::map<std::string, void*> system_vars;


	Program(const std::string& code);
	virtual ~Program();

	/*
	 * Compile the program with a VM, a context (json) and an execution mode
	 */
	double compile(VM& vm, const std::string& context, const ExecMode);

	/*
	 * Execute the program and get a LSValue* result
	 */
	LSValue* execute();

	void print(std::ostream& os, bool debug = false) const;
};

std::ostream& operator << (std::ostream& os, const Program* program);

}

#endif
