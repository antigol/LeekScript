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
	code("['', Boolean()]").equals("['', false]");
	code("['', new Boolean()]").equals("['', false]");
	code("['', new Boolean]").equals("['', false]");

	section("Boolean.operator |x|");
	code("|true|").equals("1");
	code("|false|").equals("0");

	section("Boolean.operator !");
	code("!true").equals("false");
	code("!false").equals("true");
	// TODO
	// code("!!!!!false").equals("true");
	code("var a = [true, ''] var b = a[0]; !b").equals("false");
	code("var a = true ['', !a]").equals("['', false]");

	section("Boolean.operator -");
	code("var a = [true, ''] var b = a[0]; -b").equals("-1");
	code("var a = [false, ''] var b = a[0]; -b").equals("0");

	section("Boolean.operator ~");
	code("~true").equals("-2");
	code("~false").equals("-1");
	code("var a = [true, ''] var b = a[0]; ~b").equals("-2");
	code("var a = [false, ''] var b = a[0]; ~b").equals("-1");

	section("Boolean.operator ==");
	code("true == false").equals("false");
	code("false == false").equals("true");

	section("Boolean.operator !=");
	code("true != false").equals("true");
	code("true is not true").equals("false");

	section("Boolean.operator +");
	code("true + 'salut'").equals("'truesalut'");
	code("false + '!'").equals("'false!'");
	code("let s = '!' false + s").equals("'false!'");
	code("false + 12").equals("12");
	code("true + 1").equals("2");
	code("true + false").equals("1");
	code("var a = [false, ''] var b = a[0]; b + 12").equals("12");
	code("var a = [true, ''] var b = a[0]; b + 12").equals("13");
	code("var a = [false, ''] var b = a[0]; b + true").equals("1");
	code("var a = [true, ''] var b = a[0]; b + false").equals("1");
	code("var a = [false, ''] var b = a[0]; b + false").equals("0");
	code("var a = [true, ''] var b = a[0]; b + true").equals("2");
	code("var a = [false, ''] var b = a[0]; b + ' !'").equals("'false !'");
	code("var a = [true, ''] var b = a[0]; b + ' !'").equals("'true !'");
	code("var a = [true, ''] var b = a[0]; b + []").equals("null");
	code("var a = [true, ''][0] var b = [12, ''][0] a + b").equals("13");
	code("var a = [false, ''][0] var b = [12, ''][0] a + b").equals("12");
	code("var a = [false, ''][0] a + 12").equals("12");
	code("true / 'hello'").semantic_error(ls::SemanticError::Type::NO_SUCH_OPERATOR, {ls::Type::BOOLEAN.to_string(), "/", ls::Type::STRING_TMP.to_string()});

	section("Boolean.operator -");
	code("true - 1").equals("0");
	code("1 - true").equals("0");
	code("true - false").equals("1");
	code("[true, ''][0] - 5").equals("-4");
	code("var a = [true, ''] var b = a[0]; b - 12").equals("-11");
	code("var a = [false, ''] var b = a[0]; b - false").equals("0");
	code("var a = [true, ''] var b = a[0]; b - false").equals("1");
	code("var a = [true, ''] var b = a[0]; b - 'yolo'").equals("null");
	code("var a = [true, ''][0] var b = [12, ''][0] a - b").equals("-11");
	code("var a = [false, ''][0] var b = [16, ''][0] a - b").equals("-16");
	code("var a = [false, ''][0] var b = [12, ''][0] a - b").equals("-12");
	code("var a = [false, ''][0] a - 12").equals("-12");

	section("Boolean.operator *");
	code("12 * true").equals("12");
	code("12 * false").equals("0");
	code("true * 5").equals("5");
	code("true * 5.5").equals("5.5");
	code("false * 5.5").equals("0");
	code("false * true").equals("0");
	code("512 * (5 < 7)").equals("512");

	section("operator &&");
	code("true and true").equals("true");
	code("true and false").equals("false");
	code("false and true").equals("false");
	code("false and false").equals("false");
	code("var i = 0, m = 0m i++ && m = 1m").equals("false");

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
	// TODO
	// code("[true, ''][0].compare([false, ''][0])").equals("1");
	// code("[true, ''][0].compare([true, ''][0])").equals("0");
	// code("[false, ''][0].compare([false, ''][0])").equals("0");
	// code("[false, ''][0].compare([true, ''][0])").equals("-1");

	section("Conversions");
	code("12 and 'aaa'").equals("true");
	code("[1, 2] and false").equals("false");
	code("[] and ''").equals("false");
	code("false and (x -> x)").equals("false");
	code("12 or 'aaa'").equals("true");
	code("[1, 2] or false").equals("true");
	code("false or (x -> x)").equals("true");
	code("[] or ''").equals("false");

	section("Boolean is a number");
	code("false + 99").equals("99");
	code("true - 99").equals("-98");
	code("12 + true").equals("13");
	code("15 - true").equals("14");
	code("15 * true").equals("15");
}
