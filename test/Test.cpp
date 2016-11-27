#include "Test.hpp"
#include <fstream>
#include <string>

Test::Test() {
	total = 0;
	success_count = 0;
	exeTime = 0;
	obj_deleted = 0;
	obj_created = 0;
	gmp_obj_deleted = 0;
	gmp_obj_created = 0;
}

Test::~Test() {}

int main(int, char**) {
	srand(time(0));
	return Test().all();
}

int Test::all() {

	clock_t begin = clock();
	exeTime = 0;

	test_general();
	test_booleans();
	test_numbers();
	test_strings();
	test_arrays();
	test_intervals();
	test_map();
	test_set();
	test_objects();
	test_functions();
	test_classes();
	test_loops();
	test_operators();
	test_references();
	test_operations();
	test_system();
	test_files();
	test_doc();

	double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;
	int errors = (total - success_count);
	int leaks = (obj_created - obj_deleted);
	int gmp_leaks = (gmp_obj_created - gmp_obj_deleted);

	std::cout << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Total : " << total << ", success : " << success_count << ", errors : " << errors << std::endl;
	std::cout << "Total time : " << elapsed_secs * 1000 << " ms, execution time : " << (exeTime / CLOCKS_PER_SEC) * 1000 << " ms" << std::endl;
	std::cout << "Objects destroyed : " << obj_deleted << " / " << obj_created << " (" << leaks << " leaked)" << std::endl;
	std::cout << "GMP objects destroyed : " << gmp_obj_deleted << " / " << gmp_obj_created << " (" << gmp_leaks << " leaked)" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	int result = abs(errors) + abs(leaks) + abs(gmp_leaks);
	if (result == 0) {
		std::cout << "GOOD :)" << std::endl;
	} else {
		std::cout << "BAD : " << result << " error(s) :(" << std::endl;
	}
	std::cout << "------------------------------------------------" << std::endl;

	return result;
}

Test::Input Test::_code(const std::string& code) {
	return Test::Input(this, code, code);
}

Test::Input Test::file(const std::string& file_name) {
	std::ifstream ifs(file_name);
	std::string code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	return Test::Input(this, file_name, code, true);
}

ls::VM::Result Test::Input::run(bool display_errors) {
	test->total++;

	ls::VM::operation_limit = this->operation_limit;
	auto result = test->vm.execute(_code, "{}");
	ls::VM::operation_limit = ls::VM::DEFAULT_OPERATION_LIMIT;

	test->obj_created += result.objects_created;
	test->obj_deleted += result.objects_deleted;
	test->gmp_obj_created += result.gmp_objects_created;
	test->gmp_obj_deleted += result.gmp_objects_deleted;

	compilation_time = round((float) result.compilation_time / 1000) / 1000;
	execution_time = round((float) result.execution_time / 1000) / 1000;

	if (display_errors) {
		for (const auto& error : result.syntaxical_errors) {
			std::cout << "Line " << error->message << std::endl;
		}
		for (const auto& error : result.semantical_errors) {
			std::cout << "Line " << error.line << ": " << error.message() << std::endl;
		}
	}
	#if DEBUG
		std::cout << "pgrm() " << result.program << std::endl;
	#endif
	return result;
}

#define GREY "\033[0;90m"
#define GREEN "\033[0;32m"
#define RED "\033[1;31m"
#define END_COLOR "\033[0m"

void Test::Input::pass(std::string expected) {
	std::cout << GREEN << "OK   " << END_COLOR << ": " << name()
	<<  "  ===>  " << expected;
	std::cout <<  GREY << " (" << this->compilation_time << " ms + " << this->execution_time << " ms)" << END_COLOR;
	std::cout << std::endl;
	test->success_count++;
}

void Test::Input::fail(std::string expected, std::string actual) {
	std::cout << RED << "FAIL " << END_COLOR << ": " << name()
	<< "  =/=>  " << expected << "  got  " << actual;
	std::cout <<  GREY << " (" << this->compilation_time << " ms + " << this->execution_time << " ms)" << END_COLOR;
	std::cout << std::endl;
}

void Test::Input::works() {
	std::cout << "Try " << code << " ..." << std::endl;
	try {
		run();
	} catch (...) {
		fail("works", "threw exception!");
		return;
	}
	pass("works");
}

void Test::Input::_equals(std::string&& expected) {

	auto result = run();

	if (result.semantical_errors.size()) {
		for (const auto& error : result.semantical_errors) {
			std::cout << "Semantic error: " << error.message() << std::endl;
		}
	}
	if (result.value == expected) {
		pass(expected);
	} else {
		fail(expected, result.value);
	}
}

template void Test::Input::almost(long expected, long delta);
template void Test::Input::almost(double expected, double delta);

template <typename T>
void Test::Input::almost(T expected, T delta) {

	auto result = run();

	T res_num;
	std::stringstream ss(result.value);
	ss >> res_num;

	if (std::abs(res_num - expected) <= delta) {
		pass(result.value);
	} else {
		fail(ls::LSNumber::print(expected), result.value);
	}
}

template <typename T>
void Test::Input::between(T a, T b) {

}

void Test::Input::semantic_error(ls::SemanticError::Type expected_type, std::string token) {

	auto result = run(false);

	std::string expected_message = ls::SemanticError::build_message(expected_type, token);

	if (result.semantical_errors.size()) {
		ls::SemanticError e = result.semantical_errors[0];
		if (expected_type != e.type or token != e.content) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(expected_message, "(no exception)");
	}
}

void Test::Input::lexical_error(ls::LexicalError::Type expected_type) {

	auto result = run(false);

	std::string expected_message = ls::LexicalError::build_message(expected_type);

	if (result.lexical_errors.size()) {
		ls::LexicalError e = result.lexical_errors[0];
		if (expected_type != e.type) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(expected_message, "(no exception)");
	}
}

void Test::Input::exception(ls::VM::Exception expected) {

	auto result = run(false);

	std::string expected_message = ls::VM::exception_message(expected);
	std::string actual_message = ls::VM::exception_message(result.exception);

	if (result.exception != expected) {
		fail(expected_message, actual_message);
	} else {
		pass("throw exception « " + expected_message + " »");
	}
}

void Test::Input::operations(int expected) {

	auto result = run();

	if (result.operations != expected) {
		fail(std::to_string(expected), std::to_string(result.operations));
	} else {
		pass(std::to_string(result.operations));
	}
}
Test::Input& Test::Input::timeout(int) {
	return *this;
}
Test::Input&  Test::Input::ops_limit(long int ops) {
	this->operation_limit = ops;
	return *this;
}

void Test::header(std::string text) {
	std::cout << "╔";
	for (unsigned i = 0; i < text.size() + 2; ++i) std::cout << "═";
	std::cout << "╗" << std::endl;
	std::cout << "║ " << text << " ║" << std::endl;
	std::cout << "╚";
	for (unsigned i = 0; i < text.size() + 2; ++i) std::cout << "═";
	std::cout << "╝";
	std::cout << std::endl;
}

void Test::section(std::string text) {
	std::cout << "┌";
	for (unsigned i = 0; i < text.size() + 2; ++i) std::cout << "─";
	std::cout << "┐" << std::endl;
	std::cout << "│ " << text << " │" << std::endl;
	std::cout << "└";
	for (unsigned i = 0; i < text.size() + 2; ++i) std::cout << "─";
	std::cout << "┘";
	std::cout << std::endl;
}
