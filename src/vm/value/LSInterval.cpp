#include <exception>

#include "LSInterval.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

using namespace std;

namespace ls {

LSInterval::LSInterval() {}

LSInterval::~LSInterval() {}

LSArray<int>* LSInterval::ls_filter(LSFunction* function) {
	auto fun = (bool (*)(void*, int)) function->function;

	LSArray<int>* new_array = new LSArray<int>();

	int i = this->a;
	while (i <= b) {
		if (fun(function, i)) new_array->push_clone(i);
		i++;
	}
	if (this->refs == 0) delete this;
	return new_array;
}

bool LSInterval::in_v(int value) const {
	bool res = value >= a and value <= b;
	LSValue::delete_temporary(this);
	return res;
}

int LSInterval::atv(const int key) const {
	return a + key;
}

LSValue* LSInterval::at(const LSValue* key) const {

	return LSNumber::get(a + ((LSNumber*) key)->value);
}

LSValue** LSInterval::atL(const LSValue*) {
	// TODO
	return nullptr;
}

LSValue* LSInterval::abso() const {
	return LSNumber::get(b - a + 1);
}

LSValue* LSInterval::clone() const {

	LSInterval* new_interval = new LSInterval();
	new_interval->a = a;
	new_interval->b = b;
	return new_interval;
}

std::ostream& LSInterval::print(std::ostream& os) const {
	os << "[" << a << ".." << b << "]";
	return os;
}

}
