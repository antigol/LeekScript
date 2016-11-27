#include "Test.hpp"

void Test::test_booleans() {

	header("Booleans");

	section("Constructor");
	code("Boolean()").equals("false");
	code("[Boolean(), 'str']").equals("[false, 'str']");
	code("new Boolean").equals("false");
	code("new Boolean()").equals("false");
	code("true").equals("true");
	code("false").equals("false");

	section("operator !");
	code("!true").equals("false");
	code("!false").equals("true");

	section("Boolean.operator +");
	code("true + 'salut'").equals("'truesalut'");
	code("false + '!'").equals("'false!'");
//	code("false + 12").semantic_error(ls::SemanticError::Type::NO_SUCH_OPERATOR, "+");

	section("operator &&");
	code("true and true").equals("true");
	code("true and false").equals("false");
	code("false and true").equals("false");
	code("false and false").equals("false");

	section("operator ||");
	code("true or true").equals("true");
	code("true or false").equals("true");
	code("false or true").equals("true");
	code("false or false").equals("false");

	section("operator xor");
	code("true xor true").equals("false");
	code("true xor false").equals("true");
	code("false xor true").equals("true");
	code("false xor false").equals("false");
	code("12 xor 5").equals("false");
	code("12 xor 0").equals("true");
	code("false xor 99").equals("true");
	code("false + 99").equals("99");
	code("true + 99").equals("100");

	section("Boolean.compare()");
	code("Boolean.compare(true, false)").equals("1");
	code("Boolean.compare(true, true)").equals("0");
	code("Boolean.compare(false, false)").equals("0");
	code("Boolean.compare(false, true)").equals("-1");
	code("Boolean.compare([true, ''][0], [false, ''][0])").equals("1");
	code("Boolean.compare([true, ''][0], [true, ''][0])").equals("0");
	code("Boolean.compare([false, ''][0], [false, ''][0])").equals("0");
	code("Boolean.compare([false, ''][0], [true, ''][0])").equals("-1");
	code("true.compare(false)").equals("1");
	code("true.compare(true)").equals("0");
	code("false.compare(false)").equals("0");
	code("false.compare(true)").equals("-1");
	//code("[true, ''][0].compare([false, ''][0])").equals("1");
	//code("[true, ''][0].compare([true, ''][0])").equals("0");
	//code("[false, ''][0].compare([false, ''][0])").equals("0");
	//code("[false, ''][0].compare([true, ''][0])").equals("-1");

	section("Conversions");
	code("12 and 'aaa'").equals("true");
	code("[1, 2] and false").equals("false");
	code("[] and ''").equals("false");
	code("false and (x -> x)").equals("false");
	code("12 or 'aaa'").equals("true");
	code("[1, 2] or false").equals("true");
	code("false or (x -> x)").equals("true");
	code("[] or ''").equals("false");
}
