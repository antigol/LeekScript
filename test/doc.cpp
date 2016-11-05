#include "Test.hpp"
#include "../src/doc/Documentation.hpp"

void Test::test_doc() {

	header("Documentation");
	ls::Documentation doc;
	std::ostringstream oss;
	doc.generate(oss);
	std::cout << oss.str().substr(0, 300) << " [...]" << std::endl;
}
