#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class ArrayAccess : public LeftValue {
public:

	Value* array;
	Value* key;
	Value* key2;
	Type array_element_type;
	Type map_key_type;

	ArrayAccess();
	virtual ~ArrayAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>&);
	bool array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>&, int level);
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_l(Compiler&) const override;
};

}

#endif
