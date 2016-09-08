#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>

#include "../lexical/Ident.hpp"
#include "Value.hpp"
#include "Block.hpp"
#include "../TypeName.hpp"

namespace ls {

class SemanticVar;

class Function : public Value {
public:

	bool lambda = false;
	std::vector<Token*> arguments;
	std::vector<bool> references;
	std::vector<Value*> defaultValues;
	std::vector<TypeName*> typeNames;
	TypeName* returnType;
	std::vector<SemanticVar*> captures;
	Block* body;

	std::vector<SemanticVar*> arguments_vars;
//	int pos;
//	std::map<std::string, SemanticVar*> vars;
//	bool function_added;
//	Function* parent;

	Function();
	virtual ~Function();

	void addArgument(Token* token, bool reference, TypeName* typeName, Value* defaultValue);
	void capture(SemanticVar* var);

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
