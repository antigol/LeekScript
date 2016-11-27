#include "Test.hpp"

void Test::test_intervals() {

	header("Intervals");
	code("[1..10]").equals("[1..10]");
	code("[1..Number.sqrt(1989)]").equals("[1..44]");

	section("Interval.operator in");
	code("145 in [1..1000]").equals("true");
	code("1 in [1..1000]").equals("true");
	code("1000 in [1..1000]").equals("true");
	code("0 in [1..1000]").equals("false");
	code("1001 in [1..1000]").equals("false");

	section("Interval.operator []");
	code("[0..1000][500]").equals("500");
	code("[1..1000][500]").equals("501");
	code("[1000..2000][12]").equals("1012");
	code("[0..44.523]").equals("[0..44]");
	code("['', [10..20]][1][5]").equals("15");
	code("[1..10]['hello']").semantic_error( ls::SemanticError::Type::ARRAY_ACCESS_KEY_MUST_BE_NUMBER, "<key 1>");
	// code("[1..10][-10]").equals(""); must throw exception

	section("|Interval|");
	code("|[0..1]|").equals("2");
	code("|[1..100]|").equals("100");

	section("Interval.filter()");
	code("[1..6].filter(x -> x > 3)").equals("[4, 5, 6]");
	code("[1..6].filter(x -> x < 3)").equals("[1, 2]");
	code("[1..100].filter(x -> x > 50 and x < 53)").equals("[51, 52]");
	code("[1..Number.sqrt(1989)].filter(x -> 1989 % x == 0)").equals("[1, 3, 9, 13, 17, 39]");
	code("[1..1989.sqrt()].filter(x -> !(1989 % x)).max()").equals("39");
	code("let divisors = n -> [1..n.sqrt()].filter(x -> !(n % x)) divisors(1989)").equals("[1, 3, 9, 13, 17, 39]");

	section("Interval clone()");
	code("let i = [1..10] [i]").equals("[[1..10]]");
}
