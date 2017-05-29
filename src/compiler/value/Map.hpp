#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Map : public Value {
public:
	std::shared_ptr<Token> opening_bracket;
	std::shared_ptr<Token> closing_bracket;
	std::vector<Value*> keys;
	std::vector<Value*> values;

	Map();
	virtual ~Map();

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
