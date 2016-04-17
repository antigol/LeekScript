#include "Documentation.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../standard/ArraySTD.hpp"
#include "../standard/NumberSTD.hpp"
#include "../standard/ObjectSTD.hpp"
#include "../standard/StringSTD.hpp"
using namespace std;

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(ostream& os) {

	string lang = "fr";

	vector<Module*> modules;
	modules.push_back(new ArraySTD());
	modules.push_back(new NumberSTD());
	modules.push_back(new ObjectSTD());
	modules.push_back(new StringSTD());

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		string file = "vm/doc/" + mod->name + "_" + lang + ".json";

		mod->generate_doc(os, file);
	}

	os << "}\n";
}
