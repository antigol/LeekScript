#include "Test.hpp"

void Test::test_references() {

	header("References");

	code("let a = 2 let b = a a = 4 b").equals("2");
	code("let a = 2 let b = @a a = 5 b").equals("5");
	code("let a = 2 let b = @a b = 6 a").equals("6");
	code("let a = 2 let b = @a let c = @b a = 7 c").equals("7");
	code("let a = 2 let b = @a let c = @b c = 8 a").equals("8");
/*
	code("let a = 'hello' let b = @a a = 'world' b").equals("'world'");
	code("let a = true let b = @a b = false b").equals("false");
	code("let a = true let b = @a b = false a").equals("false");

	// Type change from int to string
	//code("let a = 12 let b = @a b = 'salut' a").equals("'salut'");
	code("let a = [1, 2, 3] let b = @a b = 'salut' a").equals("'salut'");


	code("let v = 10 let a = [v] a[0]++ v").equals("10");
//	code("let v = 10 let a = [@v] a[0]++ v").equals("11");

 * */
}
