#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");
	section("General");
	file("test/code/primes.leek").equals("41538");
	file("test/code/primes_gmp.leek").equals("9591");
	file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 3.0);
	file("test/code/reachable_cells.leek").equals("383");

	section("Project Euler");
	file("test/code/euler/pe001.leek").equals("233168");
	file("test/code/euler/pe002.leek").equals("4613732");
	file("test/code/euler/pe003.leek").equals("6857");
	file("test/code/euler/pe004.leek").equals("906609");
	file("test/code/euler/pe005.leek").equals("232792560");
	file("test/code/euler/pe006.leek").equals("25164150");
	file("test/code/euler/pe007.leek").equals("104743");
	file("test/code/euler/pe008.leek").equals("23514624000");
	file("test/code/euler/pe009.leek").equals("31875000");
	file("test/code/euler/pe010.leek").equals("142913680098");
	file("test/code/euler/pe062.leek").equals("127035954683");
}
