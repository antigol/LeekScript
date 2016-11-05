#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <vector>

#include "Value.hpp"

namespace ls {

class Array : public Value {
public:

	std::vector<Value*> expressions;
	bool interval = false;

	Array();
	virtual ~Array();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	void elements_will_take(SemanticAnalyser*, const std::vector<Type>&, int level);
	virtual bool will_store(SemanticAnalyser* analyser, const Type& type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
