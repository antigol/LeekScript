#include "Test.hpp"



Test::Test() {
	total = 0;
	success_count = 0;
	exeTime = 0;
	obj_deleted = 0;
	obj_created = 0;
}

Test::~Test() {}

int Test::all() {

	clock_t begin = clock();
	exeTime = 0;

	test_general();
	test_booleans();
	test_numbers();
//	test_strings();
//	test_arrays();
//	test_map();
//	test_set();
//	test_objects();
	test_functions();
//	test_classes();
//	test_loops();
	test_operators();
//	test_references();
//	test_operations();
//	test_system();
	test_tuple();

	double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;
	int errors = (total - success_count);
	int leaks = (obj_created - obj_deleted);

	std::cout << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Total : " << total << ", succès : " << success_count << ", erreurs : " << errors << std::endl;
	std::cout << "Total time : " << elapsed_secs * 1000 << " ms, execution time : " << (exeTime / CLOCKS_PER_SEC) * 1000 << " ms" << std::endl;
	std::cout << "Objects destroyed : " << obj_deleted << " / " << obj_created << " (" << leaks << " leaked)" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	return abs(errors) + abs(leaks);
}

void Test::header(std::string text) {
	std::cout << "----------------" << std::endl;
	std::cout << text << std::endl;
	std::cout << "----------------" << std::endl;
}

void Test::success(std::string code, std::string expected) {

	total++;
	std::string res;
	try {
		res = vm.execute(code, "{}", ls::ExecMode::TEST);
	} catch (ls::SemanticException& e) {
		res = e.message();
	}
	obj_created += ls::LSValue::obj_count;
	obj_deleted += ls::LSValue::obj_deleted;

	if (res != expected) {
		std::cout << "FAUX : " << code << "  =/=>  " << expected << "  got  " << res << std::endl;
	} else if (ls::LSValue::obj_count != ls::LSValue::obj_deleted) {
		std::cout << "LEAK : " << code << "  ===>  " << expected << std::endl;
		success_count++;
	} else {
		std::cout << "OK   : " << code << "  ===>  " << expected << std::endl;
		success_count++;
	}
}

template void Test::success_almost(std::string code, long expected, long delta);
template void Test::success_almost(std::string code, double expected, double delta);

template <typename T>
void Test::success_almost(std::string code, T expected, T delta) {

	total++;
	std::string res;
	try {
		res = vm.execute(code, "{}", ls::ExecMode::TEST);
	} catch (ls::SemanticException& e) {
		res = e.message();
	}
	obj_created += ls::LSValue::obj_count;
	obj_deleted += ls::LSValue::obj_deleted;

	T res_num;
	std::stringstream ss(res);
	ss >> res_num;

	if (std::abs(res_num - expected) > delta) {
		std::cout << "FAUX : " << code << "  =/=>  " << expected << "  got  " << res << std::endl;
	} else {
		std::cout << "OK   : " << code << "  ===>  " << res << " (perfect: " << expected << ")" << std::endl;
		success_count++;
	}
}

void Test::ops(std::string code, int expected) {

	total++;

	std::string res;
	try {
		res = vm.execute(code, "{}", ls::ExecMode::TEST_OPS);
	} catch (ls::SemanticException& e) {
		res = e.message();
	}
	obj_created += ls::LSValue::obj_count;
	obj_deleted += ls::LSValue::obj_deleted;

	int ops = std::stoi(res);
	if (ops != expected) {
		std::cout << "FAUX : " << code << "  =/=>  " << expected << "  got  " << ops << std::endl;
	} else {
		std::cout << "OK   : " << code << "  ===>  " << expected << std::endl;
		success_count++;
	}
}

void Test::sem_err(std::string code, ls::SemanticException::Type expected_type, std::string token) {

	total++;

	bool exception = false;
	std::string expected_message = ls::SemanticException::build_message(expected_type, token);

	try {
		vm.execute(code, "{}", ls::ExecMode::TEST);

	} catch (ls::SemanticException& e) {
		exception = true;
		if (expected_type != e.type) {
			std::cout << "FAUX : " << code << "  =/=> " << expected_message << "  got  " << e.message() << std::endl;
		} else {
			std::cout << "OK   : " << code << "  ===> " << e.message() <<  std::endl;
			success_count++;
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	if (!exception) {
		std::cout << "FAUX : " << code << "  =/=> " << expected_message << "  got  " << "(no exception)" << std::endl;
	}

	obj_created += ls::LSValue::obj_count;
	obj_deleted += ls::LSValue::obj_deleted;
}

void Test::lex_err(std::string code, ls::LexicalError::Type expected_type) {

	total++;

	bool exception = false;
	std::string expected_message = ls::LexicalError::build_message(expected_type);

	try {
		vm.execute(code, "{}", ls::ExecMode::TEST);

	} catch (ls::LexicalError& e) {
		exception = true;
		if (expected_type != e.type) {
			std::cout << "FAUX : " << code << "  =/=> " << expected_message << "  got  " << e.message() << std::endl;
		} else {
			std::cout << "OK   : " << code << "  ===> " << e.message() <<  std::endl;
			success_count++;
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	if (!exception) {
		std::cout << "FAUX : " << code << "  =/=> " << expected_message << "  got  " << "(no exception)" << std::endl;
	}

	obj_created += ls::LSValue::obj_count;
	obj_deleted += ls::LSValue::obj_deleted;
}
