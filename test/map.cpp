#include "Test.hpp"

void Test::test_map() {

	header("Map");

	success("[:]", "[:]");

	success("[1 : 1 2 : 2]", "[1 : 1 2 : 2]");
	success("[1 : 1 2 : '2']", "[1 : 1 2 : '2']");
	success("['1' : '1' '1' : '2' '1' : '3']", "['1' : '1']");

	success("let x = [1 : 1 1 : 2 1 : 3] x.size()", "1");
	success("let x = [1 : 1 1 : 2 2 : '3'] x.size()", "2");

	success("let x = [1 : 1] x.insert(2, 2)", "true");
	success("let x = ['a' : 'a'] x.insert(2, 2) x", "[2 : 2 'a' : 'a']");
	success("let x = [1 : 'a'] x.insert(2, 3) x", "[1 : 'a' 2 : 3]");
	success("let x = ['a' : 1] x.insert(2, 3) x", "[2 : 3 'a' : 1]");
	success("let x = ['a' : 1] x.insert('a', 3)", "false");

	success("let x = [1 : 1] x.clear()", "[:]");
	success("let x = ['a' : 'a'] x.clear()", "[:]");

	success("let x = [1 : 1] x.erase(1)", "true");
	success("let x = ['a' : 'a'] x.erase('a') x", "[:]");
	success("let x = ['a' : 'a'] x.erase('b') x", "['a' : 'a']");
	success("let x = ['a' : 1] x.erase(3.14) x", "['a' : 1]");

	success("let x = [1 : 1] x.look(1,0)", "1");
	success("let x = ['a' : 'a'] x.look('a','b')", "'a'");
	success("let x = ['a' : 'a'] x.look('b','b')", "'b'");
	sem_err("let x = ['a' : 1] x.look(3.14,'a')", ls::SemanticException::METHOD_NOT_FOUND, "look");

	success("['a':'b'] == [1:1]", "false");
	success("let x = ['a' : 'b'] let y = [1 : 1] x.clear() == y.clear()", "true");

	success("['a':1 'b':2] < ['a':1 'b':3]", "true");
	success("[1:1 2:0] < [1:1 2:true]", "false");
}
