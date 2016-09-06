#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "../../compiler/value/Expression.hpp"
#include "../lexical/Ident.hpp"

namespace ls {

class Object : public Value {
public:

	std::vector<Ident*> keys;
	std::vector<Value*> values;

	Object();
	virtual ~Object();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;

	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
};

}

#endif
