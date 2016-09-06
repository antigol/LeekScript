#include <string>
#include <iostream>
#include <sstream>

#include "../src/vm/Context.hpp"
#include "../src/compiler/lexical/LexicalAnalyser.hpp"
#include "../src/compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "../src/compiler/semantic/SemanticAnalyser.hpp"
#include "../src/compiler/semantic/SemanticException.hpp"
#include "Test.hpp"

using namespace std;

void Test::test_general() {

	header("General");
	success("", "<void>");
	success(" ", "<void>"); // classic space
	success(" ", "<void>"); // unbreakable space
	success("	", "<void>"); // tab

	success("null", "null");
	success("()", "<tuple>");
	success("12", "12");
	success("true", "true");
	success("false", "false");
	success("'toto'", "'toto'");
	success("[]", "[]");
//	success("{}", "{}");
//	success("{a: 12}", "{a: 12}");
	success("{;}", "<void>");
	success("return 12", "12");
	success("return", "<void>");
	success("'a' 'b' 'c'", "'c'");

	header("Variables");
	success("let a = 2 a", "2");
//	success("let a, b, c = 1, 2, 3 c", "3");
	success("let a", "<void>");
	success("let a a", "false");
	success("let a a = 12 a", "12");
	success("let a = 1 let b = (a = 12) b", "12");
	success("let s = 'hello'", "<void>");
	success("let s = 'hello' s", "'hello'");
	success("let état = 12 état", "12");
	success("let 韭 = 'leek' 韭", "'leek'");
//	success("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨", "25");

	sem_err("a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");
	sem_err("let a = 2 let a = 5", ls::SemanticException::Type::VARIABLE_ALREADY_DEFINED, "a");

	success("let a = 12 a", "12");
	success("let a = 12 { let a = 5 } a", "12");
	success("let a = 12 let b = 0 { let a = 5 b = a } b", "5");
	sem_err("{let a = 5} a", ls::SemanticException::Type::UNDEFINED_VARIABLE, "a");

//	success("let f = function (x:vec<i32>, y) { if y { x } else { [] }} f([1], true)", "[1]");
//	success("let f = function (x:vec<i32>, y) { if y { x } else { return [42] [[]] }} f([1], false)", "[42]");
	success("return 1 return [] []", "1");
	success("if false { if true return 1 else return 2 } else { if true return 3 else return 'b' } return [] []", "3");

	success("let x = 1 let y = 'a' x = y", "'a'");
	success("let x = [1, 2, 3] x[1] = 'a' x", "[1, 'a', 3]");
	success("let x = []; let y = [[1.5], x]; y[0][0]", "1.5");
	success("let x = [] let y = null [x,y]", "[[], null]");
	success("let x = if true [0] else ['a'];  x[0] = 1 x", "[1]");
	success("let x = [] x.push('a') x", "['a']");
	success("let x = [[[]]] x[0][0].push('a') x", "[[['a']]]");
	success("let x = [] x.push([]) x[0].push(1) x", "[[1]]");

//	success("let x let y x = y y = 1 x", "0");
	success("let f = a,i -> a[i]; f([1],0)", "1");

//	success("'foo' ?? 'bar'", "'foo'");
//	success("null ?? 'bar'", "'bar'");
//	success("let a = 'foo' a ?? 'bar'", "'foo'");
//	success("let a = null a ?? 'bar'", "'bar'");
//	success("[] ?? [12]", "[]");
//	success("null ?? [12]", "[12]");
//	success("{} ?? 'default'", "{}");
//	success("null ?? 'default'", "'default'");
//	success("let a = null let b = null a ?? b ?? ':)'", "':)'");
}


