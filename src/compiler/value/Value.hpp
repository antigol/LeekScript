#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>

#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../../vm/Type.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

class Value {
public:

	Type type;
	bool constant;
	bool parenthesis = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;

	virtual void print(std::ostream&, int indent = 0, bool debug = false) const = 0;

	virtual unsigned line() const = 0;


	// this method must leave `type` in a complete state(i.e. type.is_complete())
	// this method must respect `req_type` (i.e. type.match_with_generic(req_type) != Type::VOID)
	//  otherwise it must generate an error
	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) = 0;

	// this method is a relaxation of the last one
	// `type` can be leave in a generic form (like Type::UNKNOWN or Type(RawType::VEC, { Type::UNKNOWN }) )
	// it is not mandatory to generate errors
	virtual void preanalyse(SemanticAnalyser* analyser);

	virtual jit_value_t compile(Compiler&) const = 0;

	static std::string tabs(int indent);
};

}

#endif
