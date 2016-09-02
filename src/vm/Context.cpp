#include "Context.hpp"
#include <sstream>
#include <string.h>
#include <vector>
#include "VM.hpp"
#include "../../lib/json.hpp"
using namespace std;

namespace ls {

std::vector<std::string> split(const std::string &s, std::string delim) {
	vector<std::string> v;
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos) {
		v.push_back(s.substr(start, end - start));
		start = end + delim.length();
		end = s.find(delim, start);
	}
	return v;
}

Context::Context(std::string ctx) {

//	Json value = Json::parse(ctx);

//	for (Json::iterator it = value.begin(); it != value.end(); ++it) {
//		vars.insert({it.key(), ls::LSValue::parse(it.value())});
//	}
}

Context::~Context() {}

}

