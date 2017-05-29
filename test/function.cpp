#include "Test.hpp"

void Test::test_functions() {

	header("Functions");
	code("x -> x").equals("<function>");
	code("function foo(x, y) { x + y } foo(1, 2)").equals("3");
	code("function f() { return 12 } [f(), 'str']").equals("[12, 'str']");
	code("function f() { 12 } [f(), 'str']").equals("[12, 'str']");
	code("function bar(u, v) return u * v - u / v end bar(12, 5)").equals("57.6");
	code("function bar(u, v) u * v - u / v end bar(12, 5)").equals("57.6");

	section("Can't call a value");
	code("null()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, {"null"});
	code("12()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, {"12"});
	code("'hello'()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, {"'hello'"});
	code("[1, 2, 3]()").semantic_error(ls::SemanticError::Type::CANNOT_CALL_VALUE, {"[1, 2, 3]"});

	section("Simple returns");
	code("1; 2").equals("2");
	code("return 1").equals("1");
	code("return 'a'").equals("'a'");
	code("let x = 'yolo' return '1'; 2").equals("'1'");
	code("let x = '1' return x; 2").equals("'1'");

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
	// TODO
	// code("(-> -> 12)()()").equals("12");
	// code("let f = -> -> 12 f()()").equals("12");
	// code("let f = x -> -> 'salut' f(5)()").equals("'salut'");
	code("let f = x -> [x, x, x] f(44)").equals("[44, 44, 44]");
	code("let f = function(x) { let r = x ** 2 return r + 1 } f(10)").equals("101");
	code("let f = function(x) { if (x < 10) {return true} return 12 } [f(5), f(20)]").equals("[true, 12]");
	// code("let f = x -> { let y = { if x == 0 { return 'error' } 1/x } '' + y } [f(-2), f(0), f(2)]").equals("['-0.5', 'error', '0.5']");
	code("let f = i -> { [1 2 3][i] } f(1)").equals("2");
	code("let f = i -> { [1 2 3][i] } 42").equals("42");
	code("let f = a, i -> a[i] f([1 2 3], 1)").equals("2");
	code("[x -> x][0]").equals("<function>");
	code("let f = x = 2 -> x + 1 f").equals("<function>");
	code("let f = b -> if b { 2 } else { 3 } f(false)").equals("3");
	code("let f = b => {b = !b if b { 2 } else { 3 }} f(false)").equals("2");
	// TODO leak
	// code("(x -> y -> x + 1)(1)(2)").equals("2");

	section("Function call without commas");
	code("let f = x, y -> x + y f(12 7)").equals("19");
	code("String.replace('banana' 'a' '_')").equals("'b_n_n_'");

	section("Closures");
	code("let a = 5 let f = -> a f()").equals("5");
	code("let a = 5 let f = -> @a f()").equals("5");
	// TODO
	// code("let a = 12 let f = -> -> a f()()").equals("12");
	// code("let a = 12 let f = -> -> -> -> -> a f()()()()()").equals("12");
	// code("let a = 12 let f = -> -> {let b = 5; -> -> -> a + b} f()()()()()").equals("17");
	// code("let f = x -> y -> x + y let g = f(5) g(12)").equals("17");
	// code("let a = 12 let f = x -> y -> x + y + a f(5)(2)").equals("19");
	// code("let f = x -> y -> x + y let g = f('a') g('b')").equals("'ab'");
	// code("let f = x -> y -> x + y f(5)(12)").equals("17");
	// code("let f = x -> y -> x + y f('a')('b')").equals("'ab'");
	// code("let f = x -> x (-> f(12))()").equals("12");
	// code("let f = x -> x let g = x -> f(x) g(12)").equals("12");
	// code("let g = x -> x ** 2 let f = x, y -> g(x + y) f(6, 2)").equals("64");
	code("let a = 5 let f = x -> x < a [1, 2, 3, 4, 5, 6].filter(f)").equals("[1, 2, 3, 4]");
	code("var g = x => { var y = 2; return x + y } g(10)").equals("12");
	code("let a = 12, b = 13, c = 14 let f = x -> x + a + b + c f(5)").equals("44");

	section("Recursive");
	code("let fact = x -> if x == 1 { 1 } else { fact(x - 1) * x } fact(8)").equals("40320");
	code("let fact = x -> if x == 1 { 1m } else { fact(x - 1) * x } fact(30m)").equals("265252859812191058636308480000000");
	code("let fact = x -> if x > 1 { fact(x - 1) * x } else { 1 } fact(10)").equals("3628800");
	// TODO
	// code("let fib = n -> if n <= 1 { n } else { fib(n - 1) + fib(n - 2) } fib(25)").equals("75025");

	section("Functions in array");
	// TODO
	// code("var a = [12, x -> x + 7] a[1](12)").equals("19");
	// code("let hl = [1, 'text', x -> x + 1] hl[2](hl[1]) + hl[2](hl[0])").equals("'text12'");

	section("Multiple versions of a function");
	code("let f = x -> x f(5) f('a')").equals("'a'");
	code("let f = x -> x f('a') f(5)").equals("5");
	code("let f = x -> x [f(5), f('a')]").equals("[5, 'a']");
	code("let f = x -> x [f(5), f('a'), f(5.5), f(2l)]").equals("[5, 'a', 5.5, 2]");

	/*
	 * Operators
	 */
	section("Function.operator !");
 	code("!(x -> x)").equals("false");

	section("Function.operator ~");
	code("~(x -> x)").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Function.operator ++x");
	code("++(x -> x)").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Function.operator --x");
	code("--(x -> x)").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	// section("Function.operator x++");
	// code("(x -> x)++").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	// section("Function.operator x--");
	// code("(x -> x)--").exception(ls::vm::Exception::NO_SUCH_OPERATOR);

	section("Operator ~ ");
	// code("let a = 10 a ~ x -> x ** 2").equals("100");
	// code("let a = 10.5 a ~ x -> x * 5").equals("52.5");

	section("Operator []");
	code("let f = x -> x f[2] = 5").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_CONTAINER, {"f"});

	section("Function operators");
	code("(+)(1, 2)").equals("3");
	code("(+)([1], 2)").equals("[1, 2]");
	code("(+)('test', 2)").equals("'test2'");
	// code("(-)(9, 2)").equals("7");
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
	// TODO lexical bug
	// code("\\(72, 7)").equals("10");
	code("(\\)(72, 7)").equals("10");
	code("['', **(2, 11)]").equals("['', 2048]");
	// code("let p = +; p(1, 2)").equals("3");
	// code("let p = +; p('test', 2)").equals("'test2'");
	// code("let p = -; p(9, 2)").equals("7");
	// code("let p = * p(5, 8)").equals("40");
	// code("let p = × p(5, 8)").equals("40");
	// code("let p = / p(48, 12)").equals("4");
	// code("let p = ÷ p(48, 12)").equals("4");
	// code("let p = % p(48, 5)").equals("3");
	// code("let p = ** p(2, 11)").equals("2048");
	// code("let p = \\ p(72, 7)").equals("10");
	code("+").equals("<function>");
	code("+.class").equals("<class Function>");
	code("let p = +; p.class").equals("<class Function>");

	section("Function.isTrue()");
	code("if [x -> x, 12][0] { 'ok' } else { null }").equals("'ok'");

	section("Function.operator ==");
	code("let a = x -> x; a == a").equals("true");
	code("let a = x -> x; a == 2").equals("false");

	section("Function.operator <");
	code("let a = x -> x; a < a").equals("false");
	code("[x -> x, ''][0] < 5").equals("false");

	section("STD method");
	code("String.size").equals("<function>");
	code("Number.cos").equals("<function>");

	section("Function reflexion");
	code("(x -> 12).return").equals("<class Number>");
	code("(x -> x).args").equals("[<class Value>]");
	code("Array.size((x, y, z -> x + y * z).args)").equals("3");
	// TODO
	// code("let f = x, y -> x f(12, 'salut') f.args").equals("[<class Number>, <class String>]");
	code("+.args").equals("[<class Value>, <class Value>]");
	code("+.return").equals("<class Value>");
	code("-.args").equals("[<class Value>, <class Value>]");
	code("*.args").equals("[<class Value>, <class Value>]");
	code("×.args").equals("[<class Value>, <class Value>]");
	code("/.args").equals("[<class Value>, <class Value>]");
	code("÷.args").equals("[<class Value>, <class Value>]");
	code("%.args").equals("[<class Value>, <class Value>]");
	code("**.args").equals("[<class Value>, <class Value>]");
	// TODO manage multiple versions of functions
	//code("let f = x -> x f(12) f('salut') f.args").equals("[null]");

	section("Check argument count");
	code("(x -> x)()").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, {"(x) → {\n    x\n}", "1", "0"});
	code("let f = x, y -> x + y f(5)").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, {"f", "2", "1"});
	code("let add = +; add(5, 12, 13)").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, {"add", "2", "3"});
	// TODO not the expected error
	// code("Number.abs(12, 12)").semantic_error(ls::SemanticError::Type::WRONG_ARGUMENT_COUNT, {"Number.abs", "12", "12"});

	section("Void functions");
	code("(x -> System.print(x))(43)").equals("(void)");

	section("Reference arguments");
	code("function inc(x) { x++ } var a = 12 inc(a) a").equals("12");
	code("function inc(@x) { x++ }").equals("(void)"); // TODO returns 13
	code("let inc = (@x) -> x++").equals("(void)"); // TODO
	code("var x = 1 let f = (@x = 2) + 1 f").semantic_error(ls::SemanticError::VALUE_MUST_BE_A_LVALUE, {"@x"});
	code("var x = 12 (@x) null").equals("null"); // TODO no null at the end
	code("var x = 12 (x)").equals("12");
	code("(@x, @y) -> x + y").equals("<function>");
	code("@x, @y -> x + y").equals("<function>");
	code("let f = (@x, @y) -> x + y").equals("(void)");
	code("let f = @x, @y -> x + y").equals("(void)");

	section("Default arguments");
	code("(x = 10) -> x").equals("<function>");
	code("x = 10 -> x").equals("<function>");
	code("(x = 10 -> x)").equals("<function>");
	code("let f = x = 10 -> x f()").equals("10");
	code("let f = (x = 10) -> x f()").equals("10");
	code("let f = (x = -10) -> x f()").equals("-10");
	code("let f = (x = 10) -> x f(5)").equals("5");
	code("let f = (x = 12.12) -> x f()").equals("12.12");
	code("let f = (x = 12.12) -> x f(5)").equals("5");
	code("let f = (x = 1212l) -> x f()").equals("1212");
	code("let f = (x = 1212l) -> x f(5)").equals("5");
	code("function f(x = 10) { x } f()").equals("10");
	code("let add = (x, y = 1) -> x + y add(5)").equals("6");
	code("let add = (x, y = 1) -> x + y add(5, 3)").equals("8");
	code("let add = (x, y = 1) -> x + y add()").semantic_error(ls::SemanticError::WRONG_ARGUMENT_COUNT, {"add", "2", "1"});
	code("let f = (x = 'a', y = 'b') -> x + y f()").equals("'ab'");
	code("let f = (x = 'a', y = 'b') -> x + y f(100, 12)").equals("112");
	code("let f = (x = 'a', y = 'b') -> x + y f(100)").equals("'100b'");
	code("let f = (x = 'a', y = 'b') -> x + y f(100)").equals("'100b'");
	code("let add = (x, y = 1) -> x + y var a = 12 add(a)").equals("13");
	code("let add = (x, y = 1) -> x + y var a = 12 add(a, 10)").equals("22");

	section("Default arguments : advanced");
	code("let f = (x = [1, 2, 3]) -> x.size() f()").equals("3");
	code("let f = (x = [1, 2, 3]) -> x.size() f([])").equals("0");
	code("let f = (x = [1, 2, 3]) -> x.size() f([1, 2, 3, 4, 5])").equals("5");
	// TODO wrong result
	// code("let f = (x = [1, 2, 3]) -> x.size() f('bonjour')").equals("7");

	section("Default arguments : expert");
	code("let f = (x = (y = 'abcd') -> y.size()) -> x f()").equals("<function>");
	code("let f = (x = (y = 'abcd') -> y.size()) -> x f([])").equals("[]");
	code("let f = (x = (y = 'abcd') -> y.size()) -> x f(2)").equals("2");
	// TODO
	// code("let f = (x = 'AA') -> (y = 'BB') -> x + y f()()").equals("'AABB'");
	// code("let f = (x = 'AA') -> (y = 'BB') -> x + y f()(4)").equals("'AA4'");
	// code("let f = (x = 'AA') -> (y = 'BB') -> x + y f(5)()").equals("'5BB'");
	// code("let f = (x = 'AA') -> (y = 'BB') -> x + y f(5)(4)").equals("9");

	section("Wrong syntaxes");
	code("(@2) -> 2").syntaxic_error(ls::SyntaxicalError::Type::UNEXPECTED_TOKEN, {"2"});
}
