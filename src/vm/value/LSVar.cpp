#include "LSVar.hpp"

using namespace std;

namespace ls {

LSVar::LSVar() : type(NIL)
{}

LSVar::LSVar(const LSVar& other) :
	type(other.type), data(other.data)
{
}

LSVar::LSVar(bool boolean) : type(BOOLEAN)
{
	data.boolean = boolean;
}

LSVar::LSVar(double real) : type(REAL)
{
	data.real = real;
}

LSVar::LSVar(int integer) : type(REAL)
{
	data.real = integer;
}

LSVar::LSVar(size_t integer) : type(REAL)
{
	data.real = integer;
}

LSVar::LSVar(const std::string& text) : type(TEXT)
{
	data.text = text;
}

LSVar::~LSVar()
{

}

bool LSVar::isTrue() const
{
	switch (type) {
		case BOOLEAN: return data.boolean;
		case REAL: return data.real != 0;
		case TEXT: return !data.text.empty();
		case NIL: return false;
	}
}

std::ostream&LSVar::print(std::ostream& os) const
{
	switch (type) {
		case BOOLEAN: return os << data.boolean ? "true" : "false";
		case REAL: return os << data.real;
		case TEXT: return os << data.text;
		case NIL: return os << "null";
	}
}

std::string LSVar::json() const
{
	return "";
}

LSValue*LSVar::clone() const
{
	return new LSVar(*this);
}

int LSVar::typeID() const
{
	return 0;
}

RawType LSVar::getRawType() const
{
	return RawType::VAR;
}

LSVar*LSVar::ls_minus()
{
	if (type != REAL) {
		return this;
	}
	if (refs == 0) {
		data.real = -data.real;
		return this;
	}
	return new LSVar(-data.real);
}

}
