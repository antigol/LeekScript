#include <exception>

#include "LSInterval.hpp"
#include "LSNumber.hpp"
#include "LSNull.hpp"

using namespace std;

namespace ls {

LSInterval::LSInterval() {}

LSInterval::~LSInterval() {}


bool LSInterval::in(LSValue* value) const {

	if (const LSNumber* n = dynamic_cast<const LSNumber*>(value)) {
		return n->value >= a and n->value <= b;
	}
	return false;
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
