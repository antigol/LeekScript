#include "Test.hpp"

void Test::test_functions() {

	/*
	 * Functions
	 */
	header("Functions");
	code("function foo(x, y) { x + y } foo(1, 2)").equals("3");
	code("function f() { return 12 } [f(), 'str']").equals("[12, 'str']");

	section("Can't call a value");
	code("null()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, "null");
	code("12()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, "12");
	code("'hello'()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, "'hello'");
	code("[1, 2, 3]()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, "[1, 2, 3]");

	section("Functions / Lambdas");
	code("let f = x -> x f(12)").equals("12");
	code("let f = x -> x ** 2 f(12)").equals("144");
	code("let f = x, y -> x + y f(5, 12)").equals("17");
	code("let f = -> 12 f()").equals("12");
	code("(x -> x)(12)").equals("12");
	code("(x, y -> x + y)(12, 5)").equals("17");
	code("( -> [])()").equals("[]");
	code("( -> 12)()").equals("12");
	code("let f = x -> x f(5) + f(7)").equals("12");
	code("[-> 12][0]()").equals("12");
	code("[-> 12, 'toto'][0]()").equals("12");
	code("(x -> x + 12.12)(1.01)").almost(13.13);
	code("(x -> x + 12)(1.01)").almost(13.01);
	code("[x -> x ** 2][0](12)").equals("144");
	code("[[x -> x ** 2]][0][0](12)").equals("144");
	code("[[[x -> x ** 2]]][0][0][0](12)").equals("144");
	code("[[[[[[[x -> x ** 2]]]]]]][0][0][0][0][0][0][0](12)").equals("144");
	code("(-> -> 12)()()").equals("12");
	code("let f = -> -> 12 f()()").equals("12");
	code("let f = x -> -> 'salut' f(5)()").equals("'salut'");
	code("let f = x -> [x, x, x] f(44)").equals("[44, 44, 44]");
	code("let f = function(x) { let r = x ** 2 return r + 1 } f(10)").equals("101");
	code("1; 2").equals("2");
	code("let x = 'yolo' return '1'; 2").equals("'1'");
	code("let x = '1' return x; 2").equals("'1'");
	code("let f = function(x) { if (x < 10) {return true} return 12 } [f(5), f(20)]").equals("[true, 12]");
	code("let f = x -> { let y = { if x == 0 { return 'error' } 1/x } '' + y } [f(-2), f(0), f(2)]").equals("['-0.5', 'error', '0.5']");
	code("let f = i -> { [1 2 3][i] } f(1)").equals("2");
	code("let f = i -> { [1 2 3][i] } 42").equals("42");
	code("let f = a, i -> a[i] f([1 2 3], 1)").equals("2");
	code("[x -> x][0]").equals("<function>");

	section("Closures");
	code("let a = 5 let f = -> a f()").equals("5");
	code("let f = x -> y -> x + y let g = f(5) g(12)").equals("17");
	code("let f = x -> y -> x + y f(5)(12)").equals("17");
	code("let f = x -> x (-> f(12))()").equals("12");
	code("let f = x -> x let g = x -> f(x) g(12)").equals("12");
	code("let g = x -> x ** 2 let f = x, y -> g(x + y) f(6, 2)").equals("64");
	code("let a = 5 let f = x -> x < a [1, 2, 3, 4, 5, 6].filter(f)").equals("[1, 2, 3, 4]");

	section("Recursive");
	code("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(8)").equals("40320");
	code("let fact = x -> if x == 1 { 1m } else { fact(x - 1) * x } fact(30m)").equals("265252859812191058636308480000000");
	code("let fact = x -> if x > 1 { fact(x - 1) * x } else { 1 } fact(10)").equals("3628800");
	code("let fib = n -> if n <= 1 { n } else { fib(n - 1) + fib(n - 2) } fib(25)").equals("75025");

	section("Operator ~ ");
	code("let a = 10 a ~ x -> x ** 2").equals("100");
	code("let a = 10.5 a ~ x -> x * 5").equals("52.5");

	section("Function operators");
//	code("+(1, 2)").equals("3");
//	code("+([1], 2)").equals("[1, 2]");
//	code("+('test', 2)").equals("'test2'");
//	code("-(9, 2)").equals("7");
	code("*(5, 8)").equals("40");
	code("*('test', 2)").equals("'testtest'");
	code("×(5, 8)").equals("40");
	code("×('test', 2)").equals("'testtest'");
	code("**(2, 11)").equals("2048");
	code("/(48, 12)").equals("4");
	code("/('banana', 'n')").equals("['ba', 'a', 'a']");
	code("÷(48, 12)").equals("4");
	code("÷('banana', 'n')").equals("['ba', 'a', 'a']");
	code("**(2, 11)").equals("2048");
	code("%(48, 5)").equals("3");
	code("let p = +; p(1, 2)").equals("3");
	code("let p = +; p('test', 2)").equals("'test2'");
	code("let p = -; p(9, 2)").equals("7");
	code("let p = * p(5, 8)").equals("40");
	code("let p = × p(5, 8)").equals("40");
	code("let p = / p(48, 12)").equals("4");
	code("let p = ÷ p(48, 12)").equals("4");
	code("let p = % p(48, 5)").equals("3");
	code("let p = ** p(2, 11)").equals("2048");
	code("+").equals("<function>");
	code("+.class").equals("<class Function>");
	code("let p = +; p.class").equals("<class Function>");

	section("Function.isTrue()");
	code("if [x -> x, 12][0] { 'ok' } else { null }").equals("'ok'");

	section("STD method");
	code("String.size").equals("<function>");
	code("Number.cos").equals("<function>");

	section("Function reflexion");
	code("(x -> 12).return").equals("<class Number>");
	code("(x -> x).args").equals("[null]");
	code("Array.size((x, y, z -> x + y * z).args)").equals("3");
	code("let f = x, y -> x f(12, 'salut') f.args").equals("[<class Number>, <class String>]");
	code("+.args").equals("[null, null]");
	code("-.args").equals("[null, null]");
	code("*.args").equals("[null, null]");
	code("×.args").equals("[null, null]");
	code("/.args").equals("[null, null]");
	code("÷.args").equals("[null, null]");
	code("%.args").equals("[null, null]");
	code("**.args").equals("[null, null]");
	//code("let f = x -> x f(12) f('salut') f.args").equals("[null]");

	section("Check argument count");
	code("(x -> x)()").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, "(x) → {\n    x\n}");
	code("let f = x, y -> x + y f(5)").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, "f");
	code("let add = +; add(5, 12, 13)").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, "add");

	section("Void functions");
	code("(x -> System.print(x))(43)").equals("(void)");
}
