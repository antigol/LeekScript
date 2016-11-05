#include "Test.hpp"

void Test::test_objects() {

	header("Objects");

	code("Object()").equals("{}");
	code("new Object").equals("{}");
	code("new Object()").equals("{}");
	code("{}").equals("{}");
	code("{a: 12}").equals("{a: 12}");
	code("{a: 12, b: 5}").equals("{a: 12, b: 5}");
	code("{a: {}, b: []}").equals("{a: {}, b: []}");

	code("let a = {} a").equals("{}");
	code("let a = {b: 12, c: 5} a").equals("{b: 12, c: 5}");
	code("let a = {b: 12, c: 5} a.b").equals("12");

	code("let a = {b: 12} a.b += 10").equals("22");
	code("let a = {b: 12} a.b -= 10").equals("2");
	code("let a = {b: 12} a.b *= 10").equals("120");
	code("let a = {b: 12} a.b /= 10").almost(1.2);
	code("let a = {b: 12} a.b %= 10").equals("2");

	code("let a = {a: 32, b: 'toto', c: false}; |a|").equals("3");

	code("{}.keys()").equals("[]");
	code("{a: 5, b: 'toto', c: true, d: -> 5}.keys()").equals("['a', 'b', 'c', 'd']");

	code("{}.values()").equals("[]");
	code("{a: 5, b: 'toto', c: true, d: -> 5}.values()").equals("[5, 'toto', true, <function>]");

	code("var f = obj -> obj.a f({a: 'foo'})").equals("'foo'");
//	code("var f = obj -> obj.a [f({a: 'foo'}), f({a: 'bar'})]").equals("['foo', 'bar']");
//	code("var f = obj -> obj.a [f(12), f({a: 'bar'})]").equals("[null, 'bar']");

	code("{a: 12 b: 5}").equals("{a: 12, b: 5}");
	code("{a: 12 - 2 yo: -6}").equals("{a: 10, yo: -6}");
	code("{a: 12 b: 'yo' c: true d: [1 2 3]}").equals("{a: 12, b: 'yo', c: true, d: [1, 2, 3]}");

	code("12 in {x: 5, y: 12}").equals("true");
	code("12 in {x: 5, y: 'yo'}").equals("false");

	code("'x' in {x: 5, y: 'yo'}.keys()").equals("true");
	code("'x' in {a: 5, y: 'yo'}.keys()").equals("false");

	//section("Object.map()");
	//code("{x: 12, y: 5}.map(x -> x + 1)", "{}");
}
