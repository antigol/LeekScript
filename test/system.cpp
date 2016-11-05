#include "Test.hpp"

#include <chrono>
#include "../src/compiler/semantic/SemanticError.hpp"

long get_sec_time();
long get_milli_time();
long get_micro_time();
long get_nano_time();

void Test::test_system() {

	header("System");

	code("System.version").equals("1");

	code("System.operations").equals("0");
	code("2 + 2 System.operations").equals("1");

	long sec_time = get_sec_time();
	code("System.time").almost(sec_time, 1L);

	long milli_time = get_milli_time();
	code("System.milliTime").almost(milli_time, 100L);

	long micro_time = get_micro_time();
	code("System.microTime").almost(micro_time, 100000L);

	long nano_time = get_nano_time();
	code("System.nanoTime").almost(nano_time, 100000000L);

	code("let a = System.print(12)").semantic_error(ls::SemanticError::Type::CANT_ASSIGN_VOID, "a");

	section("print()");
	code("System.print(true)").equals("null");
	code("System.print(12)").equals("null");
	code("System.print(123.456)").equals("null");
	code("System.print(1234567123456)").equals("null");
	code("System.print(12345671234561234567123456)").equals("null");
	code("System.print('salut')").equals("null");
}

long get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}
