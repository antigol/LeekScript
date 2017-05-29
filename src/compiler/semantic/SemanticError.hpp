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
		NO_SUCH_OPERATOR,
		CANT_MODIFY_CONSTANT_VALUE,
		VALUE_NOT_ITERABLE,
		NO_SUCH_ATTRIBUTE,
		VALUE_MUST_BE_A_CONTAINER
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type, std::vector<std::string> parameters);

	Type type;
	Location location;
	Location focus;
	std::vector<std::string> parameters;
	std::string underline_code;
	std::string file;

	SemanticError(Type type, Location location, Location focus);
	SemanticError(Type type, Location location, Location focus, std::vector<std::string> parameters);
	virtual ~SemanticError();

	std::string message() const;
};

}

#endif
