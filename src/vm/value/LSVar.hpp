#ifndef LSVAR_H
#define LSVAR_H

#include "../LSValue.hpp"
#include <string>

namespace ls {

class LSVar : public LSValue
{
public:	
	enum Type {
		NIL = 0,
		BOOLEAN = 1,
		REAL = 2,
		TEXT = 3
	};

	Type type;
	union {
		bool boolean;
		double real;
		std::string text;
	} data;

	LSVar();
	LSVar(const LSVar& other);
	LSVar(bool boolean);
	LSVar(double real);
	LSVar(int integer);
	LSVar(size_t integer);
	LSVar(const std::string& text);
	virtual ~LSVar();

	bool isTrue() const override;
	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual int typeID() const override;
	virtual RawType getRawType() const override;

	LSVALUE_OPERATORS

	LSVar* ls_minus();
	LSVar* ls_not();
	LSVar* ls_tilde();
	LSVar* ls_preinc(); // ++x
	LSVar* ls_inc(); // x++
	LSVar* ls_predec();
	LSVar* ls_dec();
	LSVar* ls_abso();

	LSVar* ls_add(LSVar*);
	LSVar* ls_add_eq(LSVar*);
	LSVar* ls_sub(LSVar*);
	LSVar* ls_sub_eq(LSVar*);
	LSVar* ls_mul(LSVar*);
	LSVar* ls_mul_eq(LSVar*);
	LSVar* ls_div(LSVar*);
	LSVar* ls_div_eq(LSVar*);
	LSVar* ls_pow(LSVar*);
	LSVar* ls_pow_eq(LSVar*);
	LSVar* ls_mod(LSVar*);
	LSVar* ls_mod_eq(LSVar*);
};

}
#endif // LSVAR_H
