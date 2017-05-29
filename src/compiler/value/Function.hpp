#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "../lexical/Ident.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "Value.hpp"
#include "Block.hpp"

namespace ls {

class SemanticVar;

class Function : public Value {
public:

	struct Version {
		Block* body;
		LSFunction<LSValue*>* function = nullptr;
		Type type;
	};

	std::string name;
	bool lambda = false;
	std::vector<std::shared_ptr<Token>> arguments;
	std::vector<bool> references;
	std::vector<Value*> defaultValues;
	Block* body;

	std::vector<std::shared_ptr<SemanticVar>> captures;
	int pos;
	std::map<std::string, std::shared_ptr<SemanticVar>> vars;
	bool function_added;
	Function* parent;
	Type placeholder_type;
	bool is_main_function = false;
	std::string file;
	Version* default_version = nullptr;
	std::map<std::vector<Type>, Version*> versions;
	bool compiled = false;
	bool generate_default_version = false;
	Version* current_version = nullptr;

	Function();
	virtual ~Function();

	void addArgument(Token* token, bool reference, Value* defaultValue);
	int capture(std::shared_ptr<SemanticVar> var);
	Type getReturnType();

	void print_version(std::ostream& os, int indent, bool debug, const Version* version) const;
	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	bool will_take(SemanticAnalyser*, const std::vector<Type>&, int level) override;
	void analyse_body(SemanticAnalyser* analyser, std::vector<Type> args, Version* version, const Type& req_type);
	void update_function_args(SemanticAnalyser*);
	virtual Type version_type(std::vector<Type>) const override;
	virtual void must_return(SemanticAnalyser*, const Type&) override;
	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const override;
	void compile_version_internal(Compiler& c, std::vector<Type> args, Version* version) const;

	virtual Value* clone() const override;
};

}

#endif
