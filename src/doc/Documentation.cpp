#include "Documentation.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../vm/standard/ArraySTD.hpp"
#include "../vm/standard/MapSTD.hpp"
#include "../vm/standard/BooleanSTD.hpp"
#include "../vm/standard/NumberSTD.hpp"
#include "../vm/standard/ObjectSTD.hpp"
#include "../vm/standard/StringSTD.hpp"
using namespace std;

namespace ls {

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(ostream& os, std::string lang) {

	vector<Module*> modules;

	modules.push_back(new BooleanSTD());
	modules.push_back(new NumberSTD());
	modules.push_back(new ArraySTD());
	modules.push_back(new MapSTD());
	modules.push_back(new ObjectSTD());
	modules.push_back(new StringSTD());

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		string file = "src/doc/" + mod->name + "_" + lang + ".json";

		mod->generate_doc(os, file);
	}

	os << "}\n";

	for (const auto& m : modules) {
		delete m->clazz;
		delete m;
	}
}

}
