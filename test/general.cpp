#include <string>
#include <iostream>
#include <sstream>

#include "../src/vm/Context.hpp"
#include "../src/compiler/lexical/LexicalAnalyser.hpp"
#include "../src/compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "../src/compiler/semantic/SemanticAnalyser.hpp"
#include "../src/compiler/semantic/SemanticError.hpp"
#include "Test.hpp"

using namespace std;

void Test::test_general() {

	header("General");
	code("").equals("null");
	code(" ").equals("null"); // classic space
	code(" ").equals("null"); // unbreakable space
	code("	").equals("null"); // tab
	code("null").equals("null");
	code("()").equals("null");
	code("12").equals("12");
	code("true").equals("true");
	code("false").equals("false");
	code("'toto'").equals("'toto'");
	code("[]").equals("[]");
	code("{}").equals("{}");
	code("{a: 12}").equals("{a: 12}");
	code("{;}").equals("null");
	code("return 12").equals("12");
	code("return").equals("null");
	code("'a' 'b' 'c'").equals("'c'");

	header("Variables");
	code("let a = 2 a").equals("2");
	code("let a, b, c = 1, 2, 3 c").equals("3");
	code("let a").equals("null");
	code("let a a").equals("null");
	//code("let a a = 12 a").equals("12");
	code("let a = 1 let b = (a = 12) b").equals("12");
	code("let s = 'hello'").equals("null");
	code("let s = 'hello' s").equals("'hello'");
	code("let état = 12 état").equals("12");
	code("let 韭 = 'leek' 韭").equals("'leek'");
	code("let ♫☯🐖👽 = 5 let 🐨 = 2 ♫☯🐖👽 ** 🐨").equals("25");

	code("a").semantic_error(ls::SemanticError::Type::UNDEFINED_VARIABLE, "a");
	code("let a = 2 let a = 5").semantic_error(ls::SemanticError::Type::VARIABLE_ALREADY_DEFINED, "a");

	code("let a = 12 a").equals("12");
	code("let a = 12 { let a = 5 } a").equals("12");
	code("let a = 12 let b = 0 { let a = 5 b = a } b").equals("5");
	code("{let a = 5} a").semantic_error(ls::SemanticError::Type::UNDEFINED_VARIABLE, "a");

	code("'foo' ?? 'bar'").equals("'foo'");
	code("null ?? 'bar'").equals("'bar'");
	code("let a = 'foo' a ?? 'bar'").equals("'foo'");
	code("let a = null a ?? 'bar'").equals("'bar'");
	code("[] ?? [12]").equals("[]");
	code("null ?? [12]").equals("[12]");
	code("{} ?? 'default'").equals("{}");
	code("null ?? 'default'").equals("'default'");
	code("let a = null let b = null a ?? b ?? ':)'").equals("':)'");
}
