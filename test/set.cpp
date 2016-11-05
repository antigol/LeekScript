#include "Test.hpp"

void Test::test_set() {

	header("Set");

	code("<>").equals("<>");
	code("<2, 1, 1>").equals("<1, 2>");
	code("<'2', '1', '1'>").equals("<'1', '2'>");

	code("let s = <1, 2> s.insert(3) s").equals("<1, 2, 3>");
	code("let s = <1, 2> s.clear() s").equals("<>");
	code("let s = <1, 2> s.erase(3)").equals("false");
	code("let s = <1, 2> s.erase(1)").equals("true");
	code("let s = <1, 2> s.contains(3)").equals("false");
	code("let s = <1, 2> s.contains(1)").equals("true");
	code("let s = <1, 2> 3 in s").equals("false");
	code("let s = <1, 2> 1 in s").equals("true");

	// Type changes
	// code("let s = <1, 2> s += 'a' s").equals("<1, 2, 'a'>");

	section("Set.size()");
	code("<>.size()").equals("0");
	code("<1, 2, 3>.size()").equals("3");
	code("<1.6, 2.1, 3.75, 12.12>.size()").equals("4");
	code("<'éééé', null>.size()").equals("2");
	code("Set.size(<1, 2, 3>)").equals("3");
	code("Set.size(<1.6, 2.1, 3.75, 12.12>)").equals("4");
	code("Set.size(<'éééé', null>)").equals("2");

	section("Set clone()");
	code("let s = <1, 2, 3> [s]").equals("[<1, 2, 3>]");
}
