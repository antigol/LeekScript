#include "Util.hpp"

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <queue>

using namespace std;

string Util::read_file(string file) {
	ifstream ifs(file.data());
	string content = string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	ifs.close();
	return content;
}

vector<string> Util::read_file_lines(string file) {
	ifstream f(file);
	std::vector<std::string> lines;
	for (std::string line; std::getline(f, line);) {
		lines.push_back(line);
	}
	return lines;
}

bool Util::is_file_name(std::string data) {
	// Real file?
	std::ifstream test(data);
  	if (test) return true;
	// Contains spaces => no
	if (data.find_first_of("\t\n ") != string::npos) return false;
	// Ends with '.leek' or '.ls' => yes
	if (data.size() <= 4) return false;
	string dot_leek = ".leek";
	string dot_ls = ".ls";
    if (std::equal(dot_leek.rbegin(), dot_leek.rend(), data.rbegin())) return true;
	if (std::equal(dot_ls.rbegin(), dot_ls.rend(), data.rbegin())) return true;
	// Not a file
	return false;
}

string Util::replace_all(string& haystack, const string& needle, const string& replacement) {
	size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
         haystack.replace(pos, needle.length(), replacement);
         pos += replacement.length();
    }
    return haystack;
}

std::string Util::file_short_name(std::string path) {
	return {
		std::find_if(path.rbegin(), path.rend(), [](char c) {
			return c == '/';
		}).base(), path.end()
	};
}
