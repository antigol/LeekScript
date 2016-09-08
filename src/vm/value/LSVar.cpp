#include "LSVar.hpp"
#include <cmath>

using namespace std;

namespace ls {

LSVar::LSVar() : type(REAL), real(0.0) {}
LSVar::LSVar(const LSVar& other) : type(other.type), real(other.real), text(other.text) {}
LSVar::LSVar(const char* text) : type(TEXT), text(text) {}
LSVar::LSVar(bool boolean) : type(BOOLEAN), real(boolean) {}
LSVar::LSVar(double r) : type(REAL), real(r) {}
LSVar::LSVar(int32_t integer) : type(REAL), real(integer) {}
LSVar::LSVar(int64_t integer) : type(REAL), real(integer) {}
LSVar::LSVar(size_t integer) : type(REAL), real(integer) {}
LSVar::LSVar(const std::string& text) : type(TEXT), text(text) {}

LSVar::~LSVar()
{

}

bool LSVar::isTrue() const
{
	switch (type) {
		case BOOLEAN: return real > 0.0;
		case REAL: return real != 0;
		case TEXT: return !text.empty();
	}
}

string LSVar::to_string() const
{
	stringstream oss;
	print(oss);
	return oss.str();
}

std::ostream&LSVar::print(std::ostream& os) const
{
	switch (type) {
		case BOOLEAN: return os << (real > 0.0 ? "true" : "false");
		case REAL: return os << real;
		case TEXT: return os << '\'' << text << '\'';
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

bool LSVar::eq(const LSVar* var) const
{
	if (type != var->type) return false;
	if (type == BOOLEAN) return (real > 0.0) == (var->real > 0.0);
	if (type == REAL) return real == var->real;
	if (type == TEXT) return text == var->text;
	return false;
}

bool LSVar::lt(const LSVar* var) const
{
	if (type == BOOLEAN || type == REAL) {
		if (var->type == BOOLEAN || var->type == REAL) {
			return real < var->real;
		} else {
			return true;
		}
	} else {
		if (var->type == BOOLEAN || var->type == REAL) {
			return false;
		} else {
			return text < var->text;
		}
	}
}

LSVar* LSVar::ls_minus(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type != REAL) {
		return x;
	}
	if (x->refs == 0) {
		x->real = -x->real;
		return x;
	}
	return new LSVar(-x->real);
}

LSVar* LSVar::ls_not(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type != BOOLEAN) return x;
	if (x->refs == 0) {
		x->real = (x->real > 0.0 ? 0.0 : 1.0);
		return x;
	}
	return new LSVar(x->real > 0.0 ? 0.0 : 1.0);
}

LSVar* LSVar::ls_tilde(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type != REAL) return x;
	int i = x->real;
	if (i != x->real) return x;
	if (x->refs == 0) {
		x->real = ~i;
		return x;
	}
	return new LSVar(~i);
}

LSVar* LSVar::ls_preinc(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type == REAL) x->real += 1;
	return x;
}

LSVar* LSVar::ls_postinc(LSVar* x)
{
	if (!x) return nullptr;
	if (x->refs == 0) return x;
	if (x->type == REAL) {
		LSVar* copy = x->clone();
		x->real += 1;
		return copy;
	}
	return x;
}

LSVar* LSVar::ls_predec(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type == REAL) x->real -= 1;
	return x;
}

LSVar* LSVar::ls_postdec(LSVar* x)
{
	if (!x) return nullptr;
	if (x->refs == 0) return x;
	if (x->type == REAL) {
		LSVar* copy = x->clone();
		x->real -= 1;
		return copy;
	}
	return x;
}

LSVar* LSVar::ls_abso(LSVar* x)
{
	if (!x) return nullptr;
	if (x->type != REAL) return x;
	if (x->refs == 0) {
		x->real = fabs(x->real);
		return x;
	}
	return new LSVar(fabs(x->real));
}

LSVar* LSVar::ls_add(LSVar* x, LSVar* y)
{
	LSVar* r = nullptr;

	if (!x && !y) return r;
	if (!x) {
		if (y->type == TEXT) r = new LSVar("null" + y->text);
		if (y->refs == 0) delete y;
		return r;
	}
	if (!y) {
		if (x->type == TEXT) r = new LSVar(x->text + "null");
		if (x->refs == 0) delete x;
		return r;
	}

	if ((x->type == REAL || x->type == BOOLEAN) && (y->type == REAL || y->type == BOOLEAN)) r = new LSVar(x->real + y->real);
	else if (x->type == TEXT && y->type == TEXT) r = new LSVar(x->text + y->text);
	else if (x->type == TEXT && y->type == REAL) r = new LSVar(x->text + y->to_string());
	else if (x->type == REAL && y->type == TEXT) r = new LSVar(x->to_string() + y->text);
	else if (x->type == TEXT && y->type == BOOLEAN) r = new LSVar(x->text + (y->real > 0.0 ? "true" : "false"));
	else if (x->type == BOOLEAN && y->type == TEXT) r = new LSVar((x->real > 0.0 ? "true" : "false") + y->text);

	if (x->refs == 0) delete x;
	if (y->refs == 0) delete y;
	return r;
}

LSVar* LSVar::LSVar::ls_add_eq(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_sub(LSVar* x, LSVar* y)
{
	LSVar* r = nullptr;

	if (!x && !y) return r;
	if (!x) {
		if (y->refs == 0) delete y;
		return r;
	}
	if (!y) {
		if (x->refs == 0) delete x;
		return r;
	}

	if ((x->type == REAL || x->type == BOOLEAN) && (y->type == REAL || y->type == BOOLEAN)) r = new LSVar(x->real - y->real);

	if (x->refs == 0) delete x;
	if (y->refs == 0) delete y;
	return r;
}

LSVar* LSVar::LSVar::ls_sub_eq(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_mul(LSVar* x, LSVar* y)
{
	LSVar* r = nullptr;

	if (!x && !y) return r;
	if (!x) {
		if (y->refs == 0) delete y;
		return r;
	}
	if (!y) {
		if (x->refs == 0) delete x;
		return r;
	}

	if ((x->type == REAL || x->type == BOOLEAN) && (y->type == REAL || y->type == BOOLEAN)) r = new LSVar(x->real * y->real);

	if (x->refs == 0) delete x;
	if (y->refs == 0) delete y;
	return r;
}

LSVar* LSVar::LSVar::ls_mul_eq(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_div(LSVar* x, LSVar* y)
{
	LSVar* r = nullptr;

	if (!x && !y) return r;
	if (!x) {
		if (y->refs == 0) delete y;
		return r;
	}
	if (!y) {
		if (x->refs == 0) delete x;
		return r;
	}

	if ((x->type == REAL || x->type == BOOLEAN) && (y->type == REAL || y->type == BOOLEAN)) r = new LSVar(x->real / y->real);

	if (x->refs == 0) delete x;
	if (y->refs == 0) delete y;
	return r;
}

LSVar* LSVar::LSVar::ls_div_eq(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_pow(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_pow_eq(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_mod(LSVar* x, LSVar* y)
{

}

LSVar* LSVar::LSVar::ls_mod_eq(LSVar* x, LSVar* y)
{

}

}
