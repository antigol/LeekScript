#include "Test.hpp"

void Test::test_references() {

	header("References");
	section("Basic references with integers");
	code("var a = 2; let b = a; a = 4; b").equals("2");
	code("var a = 2; let b = @a; a = 5; b").equals("5");
	code("var a = 2; var b = @a; b = 6; a").equals("2");
	code("var a = 2; let b = @a; let c = @b; a = 7; c").equals("7");
	code("var a = 2; let b = @a; var c = @b; c = 8; a").equals("2");
	code("var a = 2; var b = 3; b = @a; a = 9; b").equals("2");

	section("Reference in expression");
	code("var a = 2; let b = (@a + 1); a = 5; b").equals("3");

	section("Reference in declaration");
	code("var a = 2, b = @a; a = 55; b").equals("55");
	code("var a = 2, b = @a; b = 56; a").equals("2");

	section("Basic references with strings");
	// TODO
	// code("var a = 'hello'; var b = @a; a = 'world'; b").equals("'hello'");
	code("var a = 'hello'; var b = @a; a += '!'; b").equals("'hello!'");
	code("var a = 'hello'; var b = @a; b += '!'; a").equals("'hello!'");
	code("var a = true; var b = @a; b = false; b").equals("false");
	code("var a = true; var b = @a; b = false; a").equals("false");

	section("Array reference");
	code("var a = [1, 2, 3]; var b = @a; b[1] = 12; a").equals("[1, 12, 3]");
	code("var a = [1, 2, 3]; var b = @a; b[1]++; a").equals("[1, 3, 3]");
	code("var a = [1, 2, 3]; var b = @a; b.clear(); a").equals("[]");
	code("var a = [1, 2, 3]; var b = @a; b.push(4); a").equals("[1, 2, 3, 4]");
	code("var a = ['a', 'b', 'c']; var b = @a; b.push([12]); a").equals("['a', 'b', 'c', [12]]");

	section("Type changes");
	// TODO
	// code("var a = 12; var b = @a; b = 'salut'; a").equals("12");
	code("var a = [1, 2, 3]; var b = @a; b = 'salut'; a").equals("[1, 2, 3]");

	section("Reference in array");
	code("var v = 10; var a = [v]; a[0]++; v").equals("10");
	code("var v = 10; var a = [@v]; a[0]++; v").equals("11");
	code("var v = 10; var a = [1, @v, 3] a[1] = 5 v").equals("10");

	section("Reference of array element");
	code("var a = ['a', 12]; var e = @a[1]; e += 5; a").equals("['a', 17]");
	code("var a = ['a', 12]; var e = @a[0]; e += 5; a").equals("['a5', 12]");
	code("var a = ['a', 12]; var e = @a[1]; a[1] -= 2; e").equals("10");
	code("var a = ['a', 12]; var e = @a[1]; a[1] *= 4; e").equals("48");
	code("var a = ['a', 12]; var e = @a[0]; a[0] += 'z'; e").equals("'az'");
	code("var a = ['a', 12]; var e = @a[1]; a[1] = 5; e").equals("12");

	section("Reference = reference");
	code("var a = 'a', b = 'b'; var c = @a; c = @b; c += '!'; [a, b]").equals("['a', 'b!']");

	section("Reference of internal variable");
	code("let a = Number a").equals("<class Number>");
	code("let a = @Number a").equals("<class Number>");
	code("@Number").equals("<class Number>");

	section("Reference convertion to pointer");
	code("var v = 2 ['', @v]").equals("['', 2]");
	code("var v = 2 ptr(@v)").equals("2");
	code("var a = 12, v = @a ['', v = 2]").equals("['', 2]");

	section("Reference on parameter");
	code("function f(a) { a++ a } f(10)").equals("11");
	code("function f(a) { var b = a b++ a } f(10)").equals("10");
	code("function f(a) { var b = @a b++ a } f(10)").equals("11");
	code("function f(a) { (@a + 2) } f(10)").equals("12");
}
