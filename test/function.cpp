#include "Test.hpp"

void Test::test_functions() {

	/*
	 * Functions
	 */
	header("Functions");
	success("function foo(x, y) { x + y } foo(1, 2)", "3");
	success("function foo(x, y) { x + y } foo(foo('Hello', ' '), 'world')", "Hello world");
	success("function foo(x, y) { y } foo(foo('Hello', ' '), 'world')", "world");
	success("function foo(x) { x = 'a' } let x = 'b' foo(x) x", "b");
	success("let f = function (x:vec<i32>, y) { if y { x } else { [] }} ls.string(f([1], true))", "[1]");
	success("let f = function (x, y) { if y { x } else { [] }} ls.string(f([1], true))", "[1]");
	success("let f = function (x) { return [42] 5 } ls.string(f([1]))", "[42]");
	success("let f = function (x:vec<i32>, y) { if y { x } else { return [42] [[]] }} ls.string(f([1], false))", "[42]");
	success("function foo() { let x = 'a' return } foo()", "<void>");

	sem_err("null()", ls::SemanticException::Type::TYPE_MISMATCH, "null");
	sem_err("12()", ls::SemanticException::Type::TYPE_MISMATCH, "12");
	sem_err("'hello'()", ls::SemanticException::Type::TYPE_MISMATCH, "'hello'");
	sem_err("[1, 2, 3]()", ls::SemanticException::Type::TYPE_MISMATCH, "[1, 2, 3]");

	/*
	 * Lambdas
	 */
	header("Functions / Lambdas");
	success("let f = x -> x f(12)", "12");
//	success("let f = x -> x ** 2 f(12)", "144");
	success("let f = x, y -> x + y f(5, 12)", "17");
	success("let f = -> 12 f()", "12");
	success("(x -> x)(12)", "12");
	success("(x, y -> x + y)(12, 5)", "17");
	success("ls.string(( -> [])())", "[]");
	success("( -> 12)()", "12");
	success("let f = x -> x f(5) + f(7)", "12");
	success("[-> 12][0]()", "12");
//	success("[-> 12, 'toto'][0]()", "12");
	success_almost("(x -> x + 12.12)(1.01)", 13.13);
	success_almost("(x -> x + 12)(1.01)", 13.01);
//	success("[x -> x ** 2][0](12)", "144");
//	success("[[x -> x ** 2]][0][0](12)", "144");
//	success("[[[x -> x ** 2]]][0][0][0](12)", "144");
//	success("[[[[[[[x -> x ** 2]]]]]]][0][0][0][0][0][0][0](12)", "144");
	success("(-> -> 12)()()", "12");
	success("let f = -> -> 12 f()()", "12");
	success("let f = x -> -> 'salut' f(true)()", "salut");
	success("let f = x -> [x, x, x] ls.string(f(44))", "[44, 44, 44]");
//	success("let f = function(x) { let r = x ** 2 return r + 1 } f(10)", "101");
	success("1; 2", "2");
	success("let x = 'leak' return '1'; 2", "1");
	success("let x = '1' return x; 2", "1");
	success("let f = function(x) { if (x < 10) {return true} return 12 } ls.string([f(5), f(20)])", "[1, 12]");
	//	success("let a = 10 a ~ x -> x ^ 2", "100");
	success("let f = x -> { let y = { if x == 0 { return 'error' } 1/x } '' + y } ls.string([f(-2.0), f(0), f(2)])", "[-0.5, error, 0.5]");
	success("let f = i:i32 -> { [1 2 3][i] } f(1)", "2");
	success("let f = i:i32 -> { [1 2 3][i] } 42", "42");
	success("let f = a:vec<i32>, i:i32 -> a[i] f([1 2 3], 1)", "2");
	success("let f = a:vec<i32> -> a[0] let g = h:fn(vec<i32>)->i32, a:vec<i32> -> h(a)  g(f,[])", "0");
//	success("let ml = a:vec<fn()->var> -> [for f in a { f() }] ls.string(ml([->'a', ->'b', ->'c']))", "[a, b, c]");
//	success("let ml = a:vec<fn()->var> -> [for f in a { f() }] let gf = -> { -> 'x'}  ls.string(ml([for i in [1,2,3] { gf() }]))", "[x, x, x]");

// with capture : success("let ml = a:vec<fn()->var> -> [for f in a { f() }] let gf = str -> { -> str}  ml([for s in ['a', 'b'] { gf(s) }])", "['a', 'b']");

	/*
	 * Closures
	 */
	header("Closures");
//		success("let a = 5 let f = -> a f()", "5");
	//	success("let f = x -> y -> x + y let g = f(5) g(12)", "17");
	//	success("let f = x -> y -> x + y f(5)(12)", "17");
	//	success("let f = x -> x (-> f(12))()", "12");
	//	success("let f = x -> x let g = x -> f(x) g(12)", "12");
	//	success("let g = x -> x ^ 2 let f = x, y -> g(x + y) f(6, 2)", "64");

	header("Recursive");
	//	success("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(10)", "3628800");

	/*
	 * Function operators
	 */
	header("Function operators");

//	success("+(1, 2)", "3");
//	success("+([1], 2)", "[1, 2]");
//	success("+('test', 2)", "'test2'");
//	success("-(9, 2)", "7");
//	success("*(5, 8)", "40");
//	success("*('test', 2)", "'testtest'");
//	success("×(5, 8)", "40");
//	success("×('test', 2)", "'testtest'");
//	success("/(48, 12)", "4");
//	success("/('banana', 'n')", "['ba', 'a', 'a']");
//	success("÷(48, 12)", "4");
//	success("÷('banana', 'n')", "['ba', 'a', 'a']");
//	success("**(2, 11)", "2048");
//	success("%(48, 5)", "3");
//	success("let p = +; p(1, 2)", "3");
//	success("let p = +; p('test', 2)", "'test2'");
//	success("let p = -; p(9, 2)", "7");
//	success("let p = * p(5, 8)", "40");
//	success("let p = × p(5, 8)", "40");
//	success("let p = / p(48, 12)", "4");
//	success("let p = ÷ p(48, 12)", "4");
//	success("let p = % p(48, 5)", "3");
//	success("let p = ** p(2, 11)", "2048");
//	success("+", "<function>");
//	success("+.class", "<class Function>");
//	success("let p = +; p.class", "<class Function>");
}
