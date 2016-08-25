#include "Test.hpp"

void Test::test_set() {

	header("Set");

	success("<>", "<>");

	success("<2, 1, 1>", "<1, 2>");
	success("<'2', '1', '1'>", "<'1', '2'>");
	success("let s = <1, 2> s.insert(3) s", "<1, 2, 3>");
	success("let s = <1, 2> s.clear() s", "<>");
	success("let s = <1, 2> s.erase(3)", "false");
	success("let s = <1, 2> s.erase(1)", "true");
	success("let s = <1, 2> s.contains(3)", "false");
	success("let s = <1, 2> s.contains(1)", "true");
	success("let s = <1, 2> 3 in s", "false");
	success("let s = <1, 2> 1 in s", "true");
	success("let s = <1, 2> s += 'a' s", "<1, 2>");
}
