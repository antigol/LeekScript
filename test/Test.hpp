#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../src/leekscript.h"
#include "../src/vm/VM.hpp"
#include "../src/compiler/semantic/SemanticError.hpp"
#include "../src/compiler/lexical/LexicalError.hpp"

/*
#define STR(...) #__VA_ARGS__
#define code(...) _code(STR(__VA_ARGS__))
#define equals(...) _equals(STR(__VA_ARGS__))
*/
#define code _code
#define equals _equals

class Test {
private:
	ls::VM vm;
	int total;
	int success_count;
	double exeTime;
	int obj_created;
	int obj_deleted;
	int gmp_obj_created;
	int gmp_obj_deleted;

public:

	class Input;

	Test();
	virtual ~Test();

	int all();
	void header(std::string);
	void section(std::string);

	Input _code(const std::string& _code);
	Input file(const std::string& file_name);

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
	void test_files();
	void test_doc();
	void test_intervals();

	class Input {
	private:
		Test* test;
		std::string _name;
		std::string _code;
		bool file;
		float time;
		long int operation_limit = ls::VM::DEFAULT_OPERATION_LIMIT;
	public:
		Input(Test* test, const std::string& name, const std::string& _code,
			bool file = false) : test(test), _name(name), _code(_code),
			file(file) {};
		void _equals(std::string&& expected);
		template <typename T>
		void almost(T expected, T delta = std::numeric_limits<T>::epsilon());
		template <typename T>
		void between(T a, T b);
		void semantic_error(ls::SemanticError::Type error, std::string param);
		void lexical_error(ls::LexicalError::Type error);
		void operations(int ops);
		void exception(ls::VM::Exception);
		Input& timeout(int ms);
		Input& ops_limit(long int ops);

		ls::VM::Result run(bool display_errors = true);
		void pass(std::string expected);
		void fail(std::string expected, std::string actuel);
		std::string& name() { return _name; };
	};
};




#endif
