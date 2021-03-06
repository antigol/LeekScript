#include "Test.hpp"

void Test::test_files() {

	/** Complex codes */
	header("Files");
	section("General");
	file("test/code/primes.leek").equals("78498");
	file("test/code/primes_gmp.leek").equals("9591");
	file("test/code/gcd.leek").equals("151");
	file("test/code/strings.leek").almost(52.0, 8.0);
	// TODO
	// file("test/code/reachable_cells.leek").equals("383");
	file("test/code/break_and_continue.leek").equals("2504");
	// TODO
	// file("test/code/french.leek").equals("'cent-soixante-huit millions quatre-cent-quatre-vingt-neuf-mille-neuf-cent-quatre-vingt-dix-neuf'");
	// TODO
	// file("test/code/french.min.leek").equals("'neuf-cent-quatre-vingt-sept milliards six-cent-cinquante-quatre millions trois-cent-vingt-et-un-mille-douze'");
	file("test/code/quine.leek").quine();
	// TODO pushAll doesn't work
	// file_v1("test/code/quine_zwik.leek").quine();
	file("test/code/dynamic_operators").works();
	file("test/code/euler1.leek").equals("2333316668");

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
	file("test/code/euler/pe010.leek").equals("142913828922");
	file("test/code/euler/pe011.leek").equals("70600674");
	file("test/code/euler/pe012.leek").equals("76576500");
	file("test/code/euler/pe013.leek").equals("5537376230");
	// TODO
	// file("test/code/euler/pe014.leek").equals("837799");
	// TODO
	// file("test/code/euler/pe015.leek").equals("137846528820");
	file("test/code/euler/pe016.leek").equals("1366");
	file("test/code/euler/pe062.leek").equals("127035954683");
	file("test/code/euler/pe063.leek").equals("49");
	file("test/code/euler/pe206.leek").equals("1389019170");
}
