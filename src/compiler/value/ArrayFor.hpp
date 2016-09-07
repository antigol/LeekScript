#ifndef ARRAYFOR_HPP
#define ARRAYFOR_HPP

#include "../value/Value.hpp"
#include "../instruction/For.hpp"

namespace ls {

class ArrayFor : public Value {
public:
	Value* forr;

	virtual ~ArrayFor();

	virtual void print(std::ostream&, int indent, bool debug) const override;
	virtual unsigned line() const override;
	virtual void analyse_help(SemanticAnalyser* analyser) override;
	virtual void reanalyse_help(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual void finalize_help(SemanticAnalyser* analyser, const Type& req_type) override;
	virtual jit_value_t compile(Compiler&) const override;
};

}
#endif // ARRAYFOR_H
