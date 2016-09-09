#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../src/leekscript.h"
#include "../src/compiler/semantic/SemanticException.hpp"
#include "../src/compiler/lexical/LexicalError.hpp"

class Test {

	ls::VM vm;
	int total;
	int success_count;
	double exeTime;
	int obj_created;
	int obj_deleted;

public:

	Test();
	virtual ~Test();

	int all();
	void header(std::string);

	void success(std::string code, std::string result);
	template <typename T>
	void success_almost(std::string code, T result, T delta = std::numeric_limits<T>::epsilon());
	void sem_err(std::string code, ls::SemanticException::Type type, std::string token);
	void lex_err(std::string code, ls::LexicalError::Type type);
	void ops(std::string code, int operations);

	void test_general();
	void test_operations();
	void test_operators();
	void test_references();
	void test_system();
	void test_objects();
	void test_strings();
	void test_numbers();
	void test_booleans();
	void test_arrays();
	void test_map();
	void test_set();
	void test_functions();
	void test_loops();
	void test_classes();
	void test_tuple();
};

#endif
