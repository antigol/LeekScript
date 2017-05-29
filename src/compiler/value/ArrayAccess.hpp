#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include <memory>
#include "LeftValue.hpp"
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ArrayAccess : public LeftValue {
public:

	Value* array;
	Value* key;
	Value* key2;
	std::shared_ptr<Token> open_bracket;
	std::shared_ptr<Token> close_bracket;
	Type array_element_type;
	Type map_key_type;
	Compiler::value compiled_array;

	ArrayAccess();
	virtual ~ArrayAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_take(SemanticAnalyser* analyser, const std::vector<Type>&, int level);
	bool array_access_will_take(SemanticAnalyser* analyser, const std::vector<Type>&, int level);
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_l(Compiler&) const override;
	virtual void compile_end(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
