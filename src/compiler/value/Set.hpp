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
	virtual unsigned line() const override;
	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual Compiler::value compile(Compiler&) const override;
};

}

#endif // SET_H
