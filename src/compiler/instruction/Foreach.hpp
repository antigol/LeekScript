#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>

#include "../lexical/Ident.hpp"
#include "../value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class Foreach : public Value {
public:

	Token* key;
	Token* value;
	Value* container;
	Block* body;
	Type key_type;
	Type value_type;
	SemanticVar* value_var;
	SemanticVar* key_var;

	Foreach();
	virtual ~Foreach();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual unsigned line() const override;

	virtual void preanalyse(SemanticAnalyser* analyser) override;
	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) override;

	virtual jit_value_t compile(Compiler&) const override;
	void compile_foreach(Compiler&c, jit_value_t container_v, jit_value_t output_v,
						 void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
						 jit_label_t* label_it, jit_label_t* label_end,
						 jit_type_t jit_value_type, jit_value_t v, jit_type_t jit_key_type, jit_value_t k) const;
	void compile_foreach_noblock(Compiler&c, jit_value_t container,
								 void* fun_begin, void* fun_condition, void* fun_value, void* fun_key, void* fun_inc,
								 jit_label_t* label_begin, jit_label_t* label_it, jit_label_t* label_block, jit_label_t* label_end,
								 jit_type_t jit_value_type, jit_value_t v, jit_type_t jit_key_type, jit_value_t k) const;
	static bool equal_type(const Type& generic, const Type& actual);
};

}

#endif
