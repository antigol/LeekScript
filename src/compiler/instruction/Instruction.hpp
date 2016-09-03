#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

//#include <ostream>
//#include "../Compiler.hpp"
//#include "../../vm/Type.hpp"

//namespace ls {

//class Value;
//class SemanticAnalyser;

//class Instruction {
//public:

//	Type type = Type::VOID;

//	virtual ~Instruction() = 0;

//	virtual void print(std::ostream&, int indent, bool debug) const = 0;

//	// this method must leave `type` in a complete state(i.e. type.is_complete())
//	// this method must respect `req_type` (i.e. type.match_with_generic(req_type) != Type::VOID)
//	//  otherwise it must generate an error
//	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) = 0;

//	virtual void preanalyse(SemanticAnalyser* analyser);

// 	virtual jit_value_t compile(Compiler&) const = 0;

// 	static std::string tabs(int indent);
//};

//}

#endif
