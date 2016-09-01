#include "LSVar.hpp"
#include <cmath>

using namespace std;

namespace ls {

LSVar::LSVar() : type(NIL)
{}

LSVar::LSVar(const LSVar& other) :
	type(other.type), real(other.real), text(other.text)
{
}

LSVar::LSVar(bool boolean) : type(BOOLEAN)
{
	real = boolean;
}

LSVar::LSVar(double r) : type(REAL)
{
	real = r;
}

LSVar::LSVar(int32_t integer) : type(REAL)
{
	real = integer;
}

LSVar::LSVar(int64_t integer) : type(REAL)
{
	real = integer;
}

LSVar::LSVar(size_t integer) : type(REAL)
{
	real = integer;
}

LSVar::LSVar(const std::string& text) : type(TEXT), text(text)
{
}

LSVar::~LSVar()
{

}

bool LSVar::isTrue() const
{
	switch (type) {
		case BOOLEAN: return real > 0.0;
		case REAL: return real != 0;
		case TEXT: return !text.empty();
		case NIL: return false;
	}
}

std::ostream&LSVar::print(std::ostream& os) const
{
	switch (type) {
		case BOOLEAN: return os << (real > 0.0 ? "true" : "false");
		case REAL: return os << real;
		case TEXT: return os << text;
		case NIL: return os << "null";
	}
}

std::string LSVar::json() const
{
	return "";
}

LSVar* LSVar::clone() const
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

bool LSVar::eq(const LSVar* var) const
{
	if (type != var->type) return false;
	if (type == NIL) return true;
	if (type == BOOLEAN) return (real > 0.0) == (var->real > 0.0);
	if (type == REAL) return real == var->real;
	if (type == TEXT) return text == var->text;
	return false;
}

bool LSVar::lt(const LSVar* var) const
{

}

LSVar*LSVar::ls_minus()
{
	if (type != REAL) {
		return this;
	}
	if (refs == 0) {
		real = -real;
		return this;
	}
	return new LSVar(-real);
}

LSVar*LSVar::ls_not()
{
	if (type != BOOLEAN) return this;
	if (refs == 0) {
		real = (real > 0.0 ? 0.0 : 1.0);
		return this;
	}
	return new LSVar(real > 0.0 ? 0.0 : 1.0);
}

LSVar*LSVar::ls_tilde()
{
	if (type != REAL) return this;
	int i = real;
	if (i != real) return this;
	if (refs == 0) {
		real = ~i;
		return this;
	}
	return new LSVar(~i);
}

LSVar*LSVar::ls_preinc()
{
	if (type == REAL) real += 1;
	return this;
}

LSVar*LSVar::ls_postinc()
{
	if (refs == 0) return this;
	if (type == REAL) {
		LSVar* copy = clone();
		real += 1;
		return copy;
	}
	return this;
}

LSVar*LSVar::ls_predec()
{
	if (type == REAL) real -= 1;
	return this;
}

LSVar*LSVar::ls_postdec()
{
	if (refs == 0) return this;
	if (type == REAL) {
		LSVar* copy = clone();
		real -= 1;
		return copy;
	}
	return this;
}

LSVar*LSVar::ls_abso()
{
	if (type != REAL) return this;
	if (refs == 0) {
		real = fabs(real);
		return this;
	}
	return new LSVar(fabs(real));
}

LSVar*LSVar::ls_add(LSVar* var)
{
	LSVar* r;
	if ((type == REAL || type == BOOLEAN) && (var->type == REAL || var->type == BOOLEAN)) r = new LSVar(real + var->real);
	else if (type == TEXT && var->type == TEXT) r = new LSVar(text + var->text);
	else if (type == TEXT && var->type == REAL) r = new LSVar(text + to_string(var->real));
	else if (type == REAL && var->type == TEXT) r = new LSVar(to_string(real) + var->text);
	else if (type == TEXT && var->type == BOOLEAN) r = new LSVar(text + (var->real > 0.0 ? "true" : "false"));
	else if (type == BOOLEAN && var->type == TEXT) r = new LSVar((real > 0.0 ? "true" : "false") + var->text);
	else if (type == TEXT && var->type == NIL) r = new LSVar(text + "null");
	else if (type == NIL && var->type == TEXT) r = new LSVar("null" + var->text);
	else r = new LSVar();

	if (refs == 0) delete this;
	if (var->refs == 0) delete var;
	return r;
}

LSVar*LSVar::ls_add_eq(LSVar* var)
{

}

LSVar*LSVar::ls_sub(LSVar* var)
{

}

LSVar*LSVar::ls_sub_eq(LSVar* var)
{

}

LSVar*LSVar::ls_mul(LSVar* var)
{

}

LSVar*LSVar::ls_mul_eq(LSVar* var)
{

}

LSVar*LSVar::ls_div(LSVar* var)
{

}

LSVar*LSVar::ls_div_eq(LSVar* var)
{

}

LSVar*LSVar::ls_pow(LSVar* var)
{

}

LSVar*LSVar::ls_pow_eq(LSVar* var)
{

}

LSVar*LSVar::ls_mod(LSVar* var)
{

}

LSVar*LSVar::ls_mod_eq(LSVar* var)
{

}

}
