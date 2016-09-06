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

///////////////// PREANALYSE ////////////////////

	// `type` must represant all possible outputs (conversions included)
	// must go through all the program
	virtual void preanalyse(SemanticAnalyser* analyser) = 0;

	// When call `will_require` ?
	//  During preanalyse, call on the non left value part
	//  x = y         called on y
	//  z = x + y     called on x,y
	virtual void will_require(SemanticAnalyser* analyser, const Type& req_type) = 0;

///////////////// ANALYSE ////////////////////

	// this method must leave `type` in a complete state (i.e. type.is_complete())
	// this method must respect `req_type` (i.e. Type::get_intersection(type, req_type))
	//  otherwise it must generate an error
	// must go through all the program
	virtual void analyse(SemanticAnalyser* analyser, const Type& req_type) = 0;

///////////////// COMPILATION ////////////////////

	virtual jit_value_t compile(Compiler&) const = 0;



	void add_error(SemanticAnalyser* analyser, SemanticException::Type error_type);
	static std::string tabs(int indent);
};

}

#endif
