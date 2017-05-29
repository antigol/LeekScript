#include "Test.hpp"
#include <fstream>
#include <string>
#include "../src/constants.h"

std::vector<std::string> Test::failed_tests;

Test::Test() : vmv1(true) {
	total = 0;
	success_count = 0;
	exeTime = 0;
	obj_deleted = 0;
	obj_created = 0;
	mpz_obj_deleted = 0;
	mpz_obj_created = 0;
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
	test_types();
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
	test_exceptions();
	test_operations();
	test_system();
	test_json();
	test_files();
	test_doc();
	test_utils();

	double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;
	int errors = (total - success_count);
	int leaks = (obj_created - obj_deleted);
	int mpz_leaks = (mpz_obj_created - mpz_obj_deleted);

	std::ostringstream line1, line2, line3, line4;
	line1 << "Total : " << total << ", success : " << success_count << ", errors : " << errors;
	line2 << "Total time : " << elapsed_secs * 1000 << " ms";
	line3 << "Objects destroyed : " << obj_deleted << " / " << obj_created << " (" << leaks << " leaked)";
	line4 << "MPZ objects destroyed : " << mpz_obj_deleted << " / " << mpz_obj_created << " (" << mpz_leaks << " leaked)";
	unsigned w = std::max(line1.str().size(), std::max(line2.str().size(), std::max(line3.str().size(), line4.str().size())));

	auto pad = [](std::string s, int l) {
		l -= s.size();
		while (l-- > 0) s += " ";
		return s;
	};
	std::cout << "┌";
	for (unsigned i = 0; i < w + 2; ++i) std::cout << "─";
	std::cout << "┐" << std::endl;
	std::cout << "│ " << pad(line1.str(), w) << " │" << std::endl;
	std::cout << "│ " << pad(line2.str(), w) << " │" << std::endl;
	std::cout << "│ " << pad(line3.str(), w) << " │" << std::endl;
	std::cout << "│ " << pad(line4.str(), w) << " │" << std::endl;
	std::cout << "├";
	for (unsigned i = 0; i < w + 2; ++i) std::cout << "─";
	std::cout << "┤";
	std::cout << std::endl;

	int result = abs(errors) + abs(leaks) + abs(mpz_leaks);
	if (result == 0) {
		std::cout << "│ " << pad("GOOD! ✔", w + 2) << " │" << std::endl;
	} else {
		std::cout << "│ " << pad("BAD! : " + std::to_string(result) + " error(s) ✘", w + 2) << " │" << std::endl;
	}
	std::cout << "└";
	for (unsigned i = 0; i < w + 2; ++i) std::cout << "─";
	std::cout << "┘" << std::endl;

	for (const auto& error : failed_tests) {
		std::cout << " " << error << std::endl;
	}
	if (failed_tests.size()) {
		std::cout << std::endl;
	}
	return result;
}

Test::Input Test::code(const std::string& code) {
	return Test::Input(this, code, code);
}

Test::Input Test::code_v1(const std::string& code) {
	return Test::Input(this, code, code, false, true);
}

Test::Input Test::file(const std::string& file_name) {
	std::ifstream ifs(file_name);
	std::string code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	return Test::Input(this, file_name, code, true);
}

Test::Input Test::file_v1(const std::string& file_name) {
	std::ifstream ifs(file_name);
	std::string code = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();
	return Test::Input(this, file_name, code, true, true);
}

ls::VM::Result Test::Input::run(bool display_errors) {
	test->total++;

	auto vm = v1 ? &test->vmv1 : &test->vm;
	vm->operation_limit = this->operation_limit;
	auto result = vm->execute(code, "{}", "test", false, true);
	vm->operation_limit = ls::VM::DEFAULT_OPERATION_LIMIT;

	this->result = result;
	test->obj_created += result.objects_created;
	test->obj_deleted += result.objects_deleted;
	test->mpz_obj_created += result.mpz_objects_created;
	test->mpz_obj_deleted += result.mpz_objects_deleted;

	compilation_time = round((float) result.compilation_time / 1000) / 1000;
	execution_time = round((float) result.execution_time / 1000) / 1000;

	if (display_errors) {
		for (const auto& error : result.syntaxical_errors) {
			std::cout << "Line " << error.type << std::endl;
		}
		for (const auto& error : result.semantical_errors) {
			std::cout << "Line " << error.location.start.line << ": " << error.message() << std::endl;
		}
	}
	#if DEBUG
		std::cout << "pgrm() " << result.program << std::endl;
	#endif
	return result;
}

void Test::Input::pass(std::string expected) {
	std::ostringstream oss;
	oss << GREEN << "OK   " << END_COLOR << ": " << name;
	if (v1) oss << BLUE << " [V1]" << END_COLOR;
	oss <<  "  ===>  " << expected;
	std::cout << oss.str();
	std::cout <<  GREY << " (" << this->compilation_time << " ms + " << this->execution_time << " ms)" << END_COLOR;
	std::cout << std::endl;
	test->success_count++;
	if (result.objects_created != result.objects_deleted) {
		oss << RED << " (" << (result.objects_created - result.objects_deleted) << " leaked)" << END_COLOR;
		failed_tests.push_back(oss.str());
	}
}

void Test::Input::fail(std::string expected, std::string actual) {
	std::ostringstream oss;
	oss << RED << "FAIL " << END_COLOR << ": " << name;
	if (v1) std::cout << BLUE << " [V1]" << END_COLOR;
	oss << "  =/=>  " << expected << "  got  " << actual;
	std::cout << oss.str();
	std::cout << GREY << " (" << this->compilation_time << " ms + " << this->execution_time << " ms)" << END_COLOR;
	std::cout << std::endl;
	failed_tests.push_back(oss.str());
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

void Test::Input::equals(std::string expected) {

	auto result = run();

	std::string errors;
	if (result.semantical_errors.size()) {
		for (const auto& error : result.semantical_errors) {
			std::cout << "Semantic error: " << error.message() << std::endl;
			errors += error.message();
		}
	}
	if (result.value == expected) {
		pass(expected);
	} else {
		auto actual = result.value;
		if (result.exception != nullptr && result.exception->type != ls::vm::Exception::NO_EXCEPTION) {
			actual = "Unexpected exception: " + ls::vm::ExceptionObj::exception_message(result.exception->type);
		}
		fail(expected, actual + errors);
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

void Test::Input::quine() {
	std::ostringstream oss;
	auto vm = v1 ? &test->vmv1 : &test->vm;
	vm->output = &oss;
	auto result = run();
	vm->output = &std::cout;

	if (oss.str() == code) {
		pass(code);
	} else {
		fail(code, oss.str());
	}
}

void Test::Input::output(std::string expected) {
	std::ostringstream oss;
	auto vm = v1 ? &test->vmv1 : &test->vm;
	vm->output = &oss;
	auto result = run();
	vm->output = &std::cout;

	if (oss.str() == expected) {
		pass(expected);
	} else {
		fail(expected, oss.str());
	}
}

template <typename T>
void Test::Input::between(T a, T b) {

}

void Test::Input::semantic_error(ls::SemanticError::Type expected_type, std::vector<std::string> parameters) {

	auto result = run(false);

	std::string expected_message = ls::SemanticError::build_message(expected_type, parameters);

	if (result.semantical_errors.size()) {
		ls::SemanticError e = result.semantical_errors[0];
		if (expected_type != e.type or parameters != e.parameters) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(expected_message, "(no semantical error)");
	}
}

void Test::Input::syntaxic_error(ls::SyntaxicalError::Type expected_type, std::vector<std::string> parameters) {

	auto result = run(false);

	std::string expected_message = ls::SyntaxicalError::build_message(expected_type, parameters);

	if (result.syntaxical_errors.size()) {
		ls::SyntaxicalError e = result.syntaxical_errors[0];
		if (expected_type != e.type or parameters != e.parameters) {
			fail(expected_message, e.message());
		} else {
			pass(e.message());
		}
	} else {
		fail(std::to_string(expected_type), "(no syntaxical error)");
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
		fail(expected_message, "(no lexical error)");
	}
}

void Test::Input::exception(ls::vm::Exception expected, std::vector<ls::vm::exception_frame> frames) {

	auto result = run(false);

	auto actual_type = result.exception != nullptr ? result.exception->type : ls::vm::Exception::NO_EXCEPTION;
	auto actual_frames = result.exception != nullptr ? result.exception->frames : std::vector<ls::vm::exception_frame>();

	if (actual_type == expected /*&& (actual_frames == frames || expected == ls::vm::Exception::NO_EXCEPTION) || actual_frames.size() == 0*/) {
		pass(result.exception != nullptr ? result.exception->to_string() : "(no exception)");
	} else {
		ls::vm::ExceptionObj expected_exception(expected);
		expected_exception.frames = frames;
		auto expected_message = expected_exception.to_string();
		auto actual_message = result.exception == nullptr ? "(no exception)" : result.exception->to_string();
		fail(expected_message, actual_message);
	}
}

void Test::Input::operations(int expected) {

	auto result = run();

	if (result.operations != expected) {
		fail(std::to_string(expected) + " ops", std::to_string(result.operations) + " ops");
	} else {
		pass(std::to_string(result.operations) + " ops");
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
