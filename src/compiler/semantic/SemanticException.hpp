#ifndef SEMANTICERROR_HPP_
#define SEMANTICERROR_HPP_

#include <string>
#include "../lexical/Token.hpp"
#include "../../../lib/json.hpp"

namespace ls {

class SemanticException : public std::exception {
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
		INDEX_TYPE,
		CANNOT_INDEX_THIS,
		VALUE_MUST_BE_A_LVALUE,
		UNKNOWN_TYPE,
		TYPE_MISMATCH,
		INCOMPATIBLE_TYPES,
		NUMBER_ARGUMENTS_MISMATCH,
		MUST_BE_ARITHMETIC_TYPE,
		RETURN_VOID,
		INFERENCE_TYPE_ERROR
	};

	static bool translation_loaded;
	static Json translation;
	static std::string type_to_string(Type);
	static std::string build_message(Type, std::string);

	Type type;
	unsigned line;
	std::string content;

	SemanticException(Type type, unsigned line = 0, const std::string& content = "");
	virtual ~SemanticException();

	std::string message() const;
};

}

#endif
