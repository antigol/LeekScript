#include "LSObject.hpp"
#include "LSNull.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

namespace ls {

LSValue* LSObject::object_class;

LSObject::LSObject() : LSValue(OBJECT) {
	clazz = nullptr;
	readonly = false;
}

LSObject::LSObject(LSClass* clazz) : LSObject() {
	this->clazz = clazz;
}

LSObject::~LSObject() {
	for (auto v : values) {
		LSValue::delete_ref(v.second);
	}
}

void LSObject::addField(string name, LSValue* var) {
	this->values.insert({name, var});
	var->refs++;
}

LSValue* LSObject::getField(std::string name) {
	return this->values.at(name);
}

LSArray<LSValue*>* LSObject::ls_get_keys() const {
	auto keys = new LSArray<LSValue*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		keys->push_inc(new LSString(i->first));
	}
	if (refs == 0) delete this;
	return keys;
}

LSArray<LSValue*>* LSObject::ls_get_values() const {
	auto v = new LSArray<LSValue*>();
	for (auto i = values.begin(); i != values.end(); i++) {
		v->push_clone(i->second);
	}
	if (refs == 0) delete this;
	return v;
}

LSObject* LSObject::ls_map(LSFunction<LSValue*>* function) const {
	auto fun = (LSValue* (*)(void*, LSValue*)) function->function;
	auto result = new LSObject();
	for (auto v : this->values) {
		auto r = fun(function, ls::clone(v.second));
		result->values.insert({v.first, r->move_inc()});
	}
	LSValue::delete_temporary(this);
	return result;
}

/*
 * LSValue methods
 */

bool LSObject::to_bool() const {
	return values.size() > 0;
}

bool LSObject::ls_not() const {
	return values.size() == 0;
}

bool LSObject::eq(const LSValue* v) const {
	if (auto obj = dynamic_cast<const LSObject*>(v)) {
		if ((!clazz && obj->clazz) || (clazz && !obj->clazz))
			return false;
		if (clazz && *clazz != *obj->clazz)
			return false;
		if (values.size() != obj->values.size())
			return false;
		auto i = values.begin();
		auto j = obj->values.begin();
		for (; i != values.end(); ++i, ++j) {
			if (i->first != j->first or *i->second != *j->second)
				return false;
		}
		return true;
	}
	return false;
}

bool LSObject::lt(const LSValue* v) const {
	if (auto obj = dynamic_cast<const LSObject*>(v)) {
		if (!clazz && obj->clazz)
			return true;
		if (clazz && !obj->clazz)
			return false;
		if (clazz && *clazz != *obj->clazz)
			return *clazz < *obj->clazz;
		auto i = values.begin();
		auto j = obj->values.begin();
		while (i != values.end()) {
			if (j == obj->values.end())
				return false;
			// i < j => true
			// j < i => false
			int x = i->first.compare(j->first);
			if (x < 0) return true;
			if (x > 0) return false;
			if (*i->second != *j->second) {
				return *i->second < *j->second;
			}
			++i; ++j;
		}
		return j != obj->values.end();
	}
	return LSValue::lt(v);
}

bool LSObject::in(const LSValue* key) const {
	for (auto i = values.begin(); i != values.end(); i++) {
		if (*i->second == *key) {
			ls::release(key);
			LSValue::delete_temporary(this);
			return true;
		}
	}
	ls::release(key);
	LSValue::delete_temporary(this);
	return false;
}

LSValue* LSObject::attr(const std::string& key) const {
	try {
		return values.at(key);
	} catch (exception& e) {
		return LSValue::attr(key);
	}
}

LSValue** LSObject::attrL(const std::string& key) {
	if (readonly) {
		jit_exception_throw(new vm::ExceptionObj(vm::Exception::CANT_MODIFY_READONLY_OBJECT));
	}
	try {
		return &values.at(key);
	} catch (exception& e) {
		values.insert({key, LSNull::get()});
		return &values[key];
	}
}

int LSObject::abso() const {
	return values.size();
}

LSValue* LSObject::clone() const {
	if (native) return (LSValue*) this;
	LSObject* obj = new LSObject();
	obj->clazz = clazz;
	for (auto i = values.begin(); i != values.end(); i++) {
		obj->values.insert({i->first, i->second->clone_inc()});
	}
	return obj;
}

std::ostream& LSObject::dump(std::ostream& os) const {
	if (clazz != nullptr) os << clazz->name << " ";
	os << "{";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) os << ", ";
		os << i->first;
		os << ": ";
		i->second->dump(os);
	}
	os << "}";
	return os;
}

std::string LSObject::json() const {
	std::string res = "{";
	for (auto i = values.begin(); i != values.end(); i++) {
		if (i != values.begin()) res += ",";
		res += "\"" + i->first + "\":";
		std::string json = i->second->json();
		res += json;
	}
	return res + "}";
}

LSValue* LSObject::getClass() const {
	if (clazz != nullptr) return clazz;
	return LSObject::object_class;
}

}
