#ifndef SET_H
#define SET_H

#include <vector>
#include "Value.hpp"

namespace ls {

class Set : public Value {
public:
	std::vector<Value*> expressions;

	Set();
	virtual ~Set();

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const override;
	virtual Location location() const override;
	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual bool will_store(SemanticAnalyser* analyser, const Type& type) override;
	
	virtual Compiler::value compile(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif // SET_H
