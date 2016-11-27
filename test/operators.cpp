#include "Test.hpp"

void Test::test_operators() {

	header("Operator =");

	//code("let a a = 2").equals("2");
	//code("let a a = 2 a").equals("2");
	code("2 = 2").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	code("'hello' = 2").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	code("[] = 2").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	code("true = []").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	code("null = x -> x").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");
	code("{} = 2.class").semantic_error(ls::SemanticError::Type::VALUE_MUST_BE_A_LVALUE, "<v>");

	header("Operator xor");

	code("true xor true").equals("false");
	code("true xor false").equals("true");
	code("false xor true").equals("true");
	code("false xor false").equals("false");

	code("12 xor 5").equals("false");
	code("12 xor 0").equals("true");
	code("false xor 99").equals("true");

	code("'salut' xor 5").equals("false");
	code("'salut' xor 'yo'").equals("false");
	code("'salut' xor false").equals("true");
	code("'salut' xor 0").equals("true");
	code("'salut' xor ''").equals("true");

	code("[] xor ''").equals("false");
	code("[] xor 777").equals("true");
	code("[1, 2, 3] xor [4, 5, 6]").equals("false");
	code("[] xor [4, 5, 6]").equals("true");

	/*
	 * Swap
	 */
	header("Swap");
	code("let a = 2 let b = 5 a <=> b [a, b]").equals("[5, 2]");
//	code("let a = [1, 2, 3, 4] a[0] <=> a[3] a").equals("[4, 2, 3, 1]");
	code("let a = 12 let b = 5 let s = a <=> b s").equals("5");

	/*
	a ~~ b => (a ~~ b)
	a ~ ~b => a ~ (~b)
	~~a => ~(~a)
	a; ~~b
	*/
	/*
	var dist = [...]
	dist ~*= 5
	var newdist = dist ~* 5

	res ?? 12 // res ? res : 12
	*/


	/*
	 * Compare
	 */

	header("Comparison");
	code("null < null").equals("false");
	code("null < true").equals("true");
	code("null < 1").equals("true");
	code("null < 'a'").equals("true");
	code("null < []").equals("true");
	code("null < {}").equals("true");

	code("true < null").equals("false");
	code("true < true").equals("false");
	code("true < 1").equals("true");
	code("true < 'a'").equals("true");
	code("true < []").equals("true");
	code("true < {}").equals("true");

	code("1 < null").equals("false");
	code("1 < true").equals("false");
	code("1 < 1").equals("false");
	code("1 < 'a'").equals("true");
	code("1 < []").equals("true");
	code("1 < {}").equals("true");

	code("'a' < null").equals("false");
	code("'a' < true").equals("false");
	code("'a' < 1").equals("false");
	code("'a' < 'a'").equals("false");
	code("'a' < []").equals("true");
	code("'a' < {}").equals("true");

	code("[] < null").equals("false");
	code("[] < true").equals("false");
	code("[] < 1").equals("false");
	code("[] < 'a'").equals("false");
	code("[] < []").equals("false");
	code("[] < {}").equals("true");

	code("{} < null").equals("false");
	code("{} < true").equals("false");
	code("{} < 1").equals("false");
	code("{} < 'a'").equals("false");
	code("{} < []").equals("false");
	code("{} < {}").equals("false");

	code("null >= null").equals("true");
	code("null >= false").equals("false");
	code("null >= 1").equals("false");
	code("null >= 'a'").equals("false");
	code("null >= []").equals("false");
	code("null >= {}").equals("false");

	code("false >= null").equals("true");
	code("false >= false").equals("true");
	code("true >= 1").equals("false");
	code("false >= 'a'").equals("false");
	code("false >= []").equals("false");
	code("false >= {}").equals("false");

	code("1 >= null").equals("true");
	code("1 >= false").equals("true");
	code("1 >= 1").equals("true");
	code("1 >= 'a'").equals("false");
	code("1 >= []").equals("false");
	code("1 >= {}").equals("false");

	code("'a' >= null").equals("true");
	code("'a' >= false").equals("true");
	code("'a' >= 1").equals("true");
	code("'a' >= 'a'").equals("true");
	code("'a' >= []").equals("false");
	code("'a' >= {}").equals("false");

	code("[] >= null").equals("true");
	code("[] >= false").equals("true");
	code("[] >= 1").equals("true");
	code("[] >= 'a'").equals("true");
	code("[] >= []").equals("true");
	code("[] >= {}").equals("false");

	code("{} >= null").equals("true");
	code("{} >= false").equals("true");
	code("{} >= 1").equals("true");
	code("{} >= 'a'").equals("true");
	code("{} >= []").equals("true");
	code("{} >= {}").equals("true");

	/*
	 * Integral division
	 */
	 header("Integral division");
	code("10 \\ 2").equals("5");
	code("10 \\ 4").equals("2");
	code("2432431 \\ 2313").equals("1051");
	code("let a = [420987, 'a', 546] a[0] \\ a[2]").equals("771");
	code("[420987, 'a', 546][0] \\ 12").equals("35082");

	/*
	 * Random operators
	 */
	section("Random operators");
	std::vector<std::string> values = {
		"0", "5", "-7", "Number.pi", "12.656", "12345678912345", "true", "false",
		"''", "'hello'", "[]", "[1, 2, 3]", "['a', 'b', 'c']", "[12, 'a', true]",
		"(x -> x)", "Number", "[:]", "[1: 2, 5: 12]", "[1:100]"
	};
	std::vector<std::string> operators = {
		"+", "-", "/", "*", ">", "<", ">=", "<=", "==", "and", "or", "xor", "\\", "in"
	};
	for (int i = 0; i < 10; ++i) {
		std::string c = values[rand() % values.size()] + " " + operators[rand() % operators.size()] + " " + values[rand() % values.size()];
		code(c).ops_limit(1000).works();
	}
}
