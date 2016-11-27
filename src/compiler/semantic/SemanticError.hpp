#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>
#include "../lexical/Token.hpp"
#include "../../../lib/json.hpp"

namespace ls {

class SemanticError {
public:

	enum Type {
		UNDEFINED_VARIABLE,
		VARIABLE_ALREADY_DEFINED,
		METHOD_NOT_FOUND,
		STATIC_METHOD_NOT_FOUND,
		CANT_ASSIGN_VOID,
		CANNOT_CALL_VALUE,
		BREAK_MUST_BE_IN_LOOP,
		CONTINUE_MUST_BE_IN_LOOP,
		ARRAY_ACCESS_KEY_MUST_BE_NUMBER,
		ARRAY_ACCESS_RANGE_KEY_MUST_BE_NUMBER,
		INVALID_MAP_KEY,
		VALUE_MUST_BE_A_LVALUE,
		WRONG_ARGUMENT_COUNT,
		NO_SUCH_OPERATOR
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type, std::string);

	Type type;
	unsigned line;
	std::string content;

	SemanticError(Type type, unsigned line);
	SemanticError(Type type, unsigned line, const std::string& content);
	virtual ~SemanticError();

	std::string message() const;
};

}

#endif
