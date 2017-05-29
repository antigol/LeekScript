#ifndef VARIABLEVALUE_HPP
#define VARIABLEVALUE_HPP

#include <memory>
#include "LeftValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

class Value;
class SemanticVar;
enum class VarScope;
class Compiler;

class VariableValue : public LeftValue {
public:

	std::string name;
	std::shared_ptr<Token> token;
	std::shared_ptr<SemanticVar> var;
	int capture_index = 0;
	VarScope scope;

	VariableValue(std::shared_ptr<Token> token);

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>&, int level) override;
	virtual bool will_store(SemanticAnalyser* analyser, const Type& type) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;
	virtual Type version_type(std::vector<Type>) const override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_l(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
