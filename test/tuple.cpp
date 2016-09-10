#include "Test.hpp"

void Test::test_tuple() {

	header("Tuples");

	success("()", "<tuple>");
	success("(1,)", "<tuple>");
	success("(1, 1.5)", "<tuple>");
//	success("(1, 1.5, 'test')", "<tuple>"); TODO

	success("(1,).0", "1");
	success("(1, 1.5).1", "1.5");
	success("(1, 1.5, 'test').1", "1.5");
	success("(1, 1.5, 'test').2", "'test'");

	success("let x = (1,) x.0", "1");
	success("let x = (1, 1.5) x.1", "1.5");
	success("let x = (1, 1.5, 'test') x.1", "1.5");
	success("let x = (1, 1.5, 'test') x.2", "'test'");

	success("let x = (1, 1.5, 'test') let y = (x, 'nested') y.0.2", "'test'");
	success("function foo(x, y) { (x, y.0) } let x = foo(('a', 'b'),('c', 'd')) x.1", "'c'");

}
