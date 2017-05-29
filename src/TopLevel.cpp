#include <algorithm>
#include <fstream>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <iterator>
#include <math.h>
#include <unistd.h>
#include <string>
#include <chrono>

#include "leekscript.h"
#include "constants.h"
#include "../lib/utf8.h"
#include "doc/Documentation.hpp"
#include "../benchmark/Benchmark.hpp"
#include "vm/LSValue.hpp"
#include "util/Util.hpp"

using namespace std;

void print_errors(ls::VM::Result& result, std::ostream& os, bool json);
void print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops);

int main(int argc, char* argv[]) {

	/** Seed random one for all */
	long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
	srand(ns);

	/** Generate the standard functions documentation */
	if (argc > 1 && string(argv[1]) == "-doc") {
		ls::Documentation().generate(cout);
		return 0;
	}

	/** Arguments */
	bool output_json = false;
	bool display_time = false;
	bool print_version = false;
	bool debug_mode = false;
	bool v1 = false;
	bool ops = true;
	bool example = false;
	std::string file_or_code;

	for (int i = 1; i < argc; ++i) {
		auto a = string(argv[i]);
		if (a == "-j" or a == "-J" or a == "--json") output_json = true;
		else if (a == "-t" or a == "-T" or a == "--time") display_time = true;
		else if (a == "-v" or a == "-V" or a == "--version") print_version = true;
		else if (a == "-d" or a == "-D" or a == "--debug") debug_mode = true;
		else if (a == "-v1" or a == "-V1") v1 = true;
		else if (a == "-nop" or a == "--no-operations") ops = false;
		else if (a == "-e" or a == "-E" or a == "--example") example = true;
		else file_or_code = a;
	}

	/** Display version? */
	if (print_version) {
		std::cout << "LeekScript 2.0" << std::endl;
		return 0;
	}
	/** Output an example code */
	if (example) {
		auto codes = Util::read_file_lines("src/doc/examples.txt");
		std::cout << codes[rand() % codes.size()] << std::endl;
		return 0;
	}

	/** Input file or code snippet? */
	if (file_or_code.size() > 0) {
		/** Get the code **/
		std::string code;
		std::string file_name;
		if (Util::is_file_name(file_or_code)) {
			ifstream ifs(file_or_code.data());
			code = string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
			ifs.close();
			file_name = Util::file_short_name(file_or_code);
		} else {
			code = file_or_code;
			file_name = "(snippet)";
		}
		/** Execute **/
		ls::VM vm {v1};
		std::ostringstream oss;
		if (output_json)
			vm.output = &oss;
		auto result = vm.execute(code, "{}", file_name, debug_mode, ops);
		vm.output = &std::cout;
		print_result(result, oss.str(), output_json, display_time, ops);
		return 0;
	}

	/** Interactive console mode */
	cout << "~~~ LeekScript v2.0 ~~~" << endl;
	string code, ctx = "{}";
	ls::VM vm(v1);

	while (!std::cin.eof()) {
		// Get a instruction
		cout << ">> ";
		std::getline(std::cin, code);
		// Execute
		auto result = vm.execute(code, ctx, "(top-level)", debug_mode, ops);
		print_result(result, "", output_json, display_time, ops);
		// Set new context
		ctx = result.context;
	}
	return 0;
}

void print_result(ls::VM::Result& result, const std::string& output, bool json, bool display_time, bool ops) {
	if (json) {
		std::ostringstream oss;
		print_errors(result, oss, json);
		std::string res = oss.str() + result.value;
		res = Util::replace_all(res, "\"", "\\\"");
		res = Util::replace_all(res, "\n", "");
		cout << "{\"success\":true,\"ops\":" << result.operations
			<< ",\"time\":" << result.execution_time
			<< ",\"ctx\":" << result.context
			<< ",\"res\":\"" << res << "\""
			<< ",\"output\":" << Json(output)
			<< "}" << endl;
	} else {
		print_errors(result, std::cout, json);
		if (result.execution_success && result.value != "(void)") {
			cout << result.value << endl;
		}
		if (display_time) {
			double compilation_time = round((float) result.compilation_time / 1000) / 1000;
			double execution_time = round((float) result.execution_time / 1000) / 1000;
			cout << GREY "(";
			if (ops) {
				cout << result.operations << " ops, ";
			}
			cout << compilation_time << "ms + " << execution_time << "ms)" << END_COLOR << endl;
		}
	}
}

void print_errors(ls::VM::Result& result, std::ostream& os, bool json) {
	for (const auto& e : result.lexical_errors) {
		os << "Line " << e.line << ": " << e.message() << std::endl;
	}
	for (const auto& e : result.syntaxical_errors) {
		os << "Line " << e.token->location.start.line << ": " << e.message() << std::endl;
	}
	bool first = true;
	for (const auto& e : result.semantical_errors) {
		if (!first) std::cout << std::endl;
		os << BOLD << e.file << ":" << e.location.start.line << END_COLOR << ": " << e.underline_code << std::endl << "   ▶ " << e.message() << std::endl;
		first = false;
	}
	if (result.exception != nullptr) {
		os << result.exception->to_string(json ? false : true);
		delete result.exception;
	}
}
