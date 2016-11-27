#ifndef FOREACH_HPP
#define FOREACH_HPP

#include <vector>

#include "../../compiler/lexical/Ident.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../compiler/value/Expression.hpp"
#include "../../compiler/value/Value.hpp"
#include "../value/Block.hpp"

namespace ls {

class Foreach : public Instruction {
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

	virtual void analyse(SemanticAnalyser*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;
	
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
