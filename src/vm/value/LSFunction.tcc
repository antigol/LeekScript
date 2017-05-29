#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"

using namespace std;

namespace ls {

template <class R>
LSClass* LSFunction<R>::clazz;

template <class R>
LSFunction<R>::LSFunction(void* function) : LSValue(FUNCTION) {
	this->function = function;
}

template <class R>
LSFunction<R>::~LSFunction() {
	for (size_t i = 0; i < captures.size(); ++i) {
		if (!captures_native[i] and captures[i] != this)
			LSValue::delete_ref(captures[i]);
	}
}

template <class R>
void LSFunction<R>::add_capture(LSValue* value) {
	if (!value->native && value != this) {
		value->refs++;
	}
	captures.push_back(value);
	captures_native.push_back(value->native);
}

template <class R>
LSValue* LSFunction<R>::get_capture(int index) {
	return captures[index];
}

/*
 * LSValue methods
 */
template <class R>
bool LSFunction<R>::to_bool() const {
	return true;
}

template <class R>
bool LSFunction<R>::ls_not() const {
	return false;
}

template <class R>
bool LSFunction<R>::eq(const LSValue* v) const {
	if (auto f = dynamic_cast<const LSFunction<LSValue*>*>(v)) {
		return function == f->function;
	}
	return false;
}

template <class R>
bool LSFunction<R>::lt(const LSValue* v) const {
	if (auto f = dynamic_cast<const LSFunction<LSValue*>*>(v)) {
		return function < f->function;
	}
	return LSValue::lt(v);
}

template <class R>
LSValue* LSFunction<R>::attr(const std::string& key) const {
	if (key == "args") {
		LSArray<LSValue*>* args_list = new LSArray<LSValue*>();
		for (const auto& arg : args) {
			args_list->push_back(arg);
		}
		return args_list;
	}
	if (key == "return") {
		return return_type;
	}
	return LSValue::attr(key);
}

template <class R>
LSValue* LSFunction<R>::clone() const {
	return (LSValue*) this;
}

template <class R>
std::ostream& LSFunction<R>::dump(std::ostream& os) const {
	os << "<function>";
	return os;
}

template <class R>
string LSFunction<R>::json() const {
	return ""; // don't output function in json
}

template <class R>
LSValue* LSFunction<R>::getClass() const {
	return LSFunction::clazz;
}

}
