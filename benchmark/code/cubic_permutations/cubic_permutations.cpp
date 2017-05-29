#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <cmath>

long code(int p) {
	std::map<std::string, int> groups;
	std::map<std::string, std::string> first;
	int i = p + 1;

	while (i++) {

	    std::string n = std::to_string(std::pow(i, 3));
		std::string ns = n;
	    std::sort(ns.begin(), ns.end());

		if (groups.find(ns) != groups.end()) {
			groups[ns]++;
			if (groups[ns] == 5) {
				return std::stol(first[ns]);
			}
		} else {
			groups.insert({ns, 1});
			first.insert({ns, n});
		}
	}
	return 0;
}

int main() {
	long s = 0;
	for (int i = 0; i < 60; ++i) {
		s += code(i);
	}
	std::cout << s << std::endl;
	return s;
}
