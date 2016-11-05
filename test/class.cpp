#include "Test.hpp"

void Test::test_classes() {

	header("Classes");

	code("null.class").equals("<class Null>");
	code("true.class").equals("<class Boolean>");
	code("false.class").equals("<class Boolean>");
	code("0.class").equals("<class Number>");
	code("12.class").equals("<class Number>");
	code("(-5).class").equals("<class Number>");
	code("12.5234.class").equals("<class Number>");
	code("''.class").equals("<class String>");
	code("'salut'.class").equals("<class String>");
	code("[].class").equals("<class Array>");
	code("[1, 2, 3].class").equals("<class Array>");
	code("{}.class").equals("<class Object>");
	code("{a: 12, b: 'yo'}.class").equals("<class Object>");
	code("(-> 12).class").equals("<class Function>");
	code("(x, y -> x + y).class").equals("<class Function>");
	code("12.class.class").equals("<class Class>");

	code("12 instanceof Number").equals("true");
	code("'yo' instanceof Number").equals("false");
	code("'yo' instanceof String").equals("true");
	code("[1, 2, 3] instanceof Array").equals("true");
	code("{a: 12} instanceof Object").equals("true");
	code("(-> 12) instanceof Function").equals("true");
	code("null instanceof Null").equals("true");
	code("true instanceof Boolean").equals("true");
	code("Number instanceof Class").equals("true");

/*
	code("class A {} let a = new A").equals("{}");
	code("class A { let b = 2 } let a = new A").equals("{}");
	code("class A { let b = 2 } let a = new A a.b").equals("2");
	code("class A { let b = 2 } let a = new A a.class").equals("<class A>");
	code("class A {} let t = [A] let a = new t[0]").equals("{}");
	code("class A {} let a = new A let b = new a.class b.class").equals("<class A>");
*/

//	code("let my_map = [].map; my_map([1, 2, 3], x -> x ^ 2)").equals("[1, 4, 9]");
//	code("[].map == [].map").equals("true");
//	code("{}.map == {}.map").equals("true");
//	code("[].map == {}.map").equals("false");
//	code("let a = [].map; a == [].map").equals("true");
}
