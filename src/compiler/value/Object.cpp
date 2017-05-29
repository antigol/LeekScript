#include "Object.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSObject.hpp"

using namespace std;

namespace ls {

Object::Object() {
	type = Type::OBJECT;
}

Object::~Object() {
	for (auto ex : values) {
		delete ex;
	}
}

void Object::print(ostream& os, int indent, bool debug) const {
	os << "{";
	for (unsigned i = 0; i < keys.size(); ++i) {
		os << keys.at(i)->content;
		os << ": ";
		values.at(i)->print(os, indent);
		if (i < keys.size() - 1) {
			os << ", ";
		}
	}
	os << "}";
	if (debug) {
		os << " " << type;
	}
}

Location Object::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Object::analyse(SemanticAnalyser* analyser, const Type&) {
	for (auto& value : values) {
		value->analyse(analyser, Type::POINTER);
	}
}

Compiler::value Object::compile(Compiler& c) const {

	auto object = c.new_object();

	for (unsigned i = 0; i < keys.size(); ++i) {
		auto k = c.new_pointer((void*) &keys.at(i)->content);
		auto v = values[i]->compile(c);
		c.insn_call(Type::VOID, {object, k, v}, +[](LSObject* o, std::string* k, LSValue* v) {
			o->addField(*k, v);
		});
	}
	return object;
}

Value* Object::clone() const {
	auto o = new Object();
	for (const auto& k : keys) {
		o->keys.push_back(k);
	}
	for (const auto& v : values) {
		o->values.push_back(v->clone());
	}
	return o;
}

}
