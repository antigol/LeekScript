#include "Test.hpp"

void Test::test_types() {

	header("Types");
	// Print unknown type nature (should not be printed elsewhere)
	std::cout << ls::Type::get_nature_symbol(ls::Nature::UNKNOWN) << std::endl;

	// Print a list of types
	std::vector<ls::Type> types { ls::Type::INTEGER, ls::Type::STRING, ls::Type::REAL_ARRAY };
	std::cout << types << std::endl;

	// Print type names
	assert(ls::Type::LONG.raw_type->getJsonName() == "number");
	assert(ls::Type::MAP.raw_type->getJsonName() == "map");
	assert(ls::Type::INTERVAL.raw_type->getJsonName() == "interval");
	assert(ls::Type::OBJECT.raw_type->getJsonName() == "object");
	assert(ls::Type::CLASS.raw_type->getJsonName() == "class");
	assert(ls::Type::SET.raw_type->getJsonName() == "set");
	assert(ls::Type::NULLL.raw_type->getJsonName() == "null");
	assert(ls::Type::VOID.raw_type->getName() == "void");
	assert(ls::Type::UNKNOWN.raw_type->getName() == "?");
	assert(ls::Type::FUNCTION.raw_type->getName() == "function");

	// Type::more_specific
	assert(ls::Type::more_specific(ls::Type::PTR_ARRAY, ls::Type::INT_ARRAY));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::INT_INT_MAP));
	assert(ls::Type::more_specific(ls::Type::PTR_PTR_MAP, ls::Type::REAL_REAL_MAP));
}
