#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "../../compiler/value/Value.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

enum class VarScope;

class Reference : public Value {
public:

	Value* value;
	std::shared_ptr<SemanticVar> var;
	int capture_index = 0;
	VarScope scope;
	std::string name;
	bool in_array = false;

	Reference(Value* value);
	virtual ~Reference();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void will_be_in_array(SemanticAnalyser*) override;
	virtual void analyse(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
