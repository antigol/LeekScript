#ifndef LSVAR_H
#define LSVAR_H

#include "../LSValue.hpp"
#include <string>

namespace ls {

class LSVar : public LSValue
{
public:	
	enum Type {
		BOOLEAN = 1,
		REAL = 2,
		TEXT = 3
	};

	Type type;
	double real;
	std::string text;

	LSVar();
	LSVar(const LSVar& other);
	LSVar(const char* text);
	LSVar(bool boolean);
	LSVar(double real);
	LSVar(int32_t integer);
	LSVar(int64_t integer);
	LSVar(size_t integer);
	LSVar(const std::string& text);
	virtual ~LSVar();

	bool isTrue() const override;
	std::string to_string() const;
	virtual std::ostream& print(std::ostream&) const override;
	virtual std::string json() const override;
	virtual LSVar* clone() const override;
	virtual int typeID() const override;
	virtual RawType getRawType() const override;

	LSVALUE_OPERATORS

	bool eq(const LSVar*) const override;
	bool lt(const LSVar*) const override;

	static LSVar* ls_minus(LSVar*);
	static LSVar* ls_not(LSVar*);
	static LSVar* ls_tilde(LSVar*);
	static LSVar* ls_preinc(LSVar*); // ++x
	static LSVar* ls_postinc(LSVar*); // x++
	static LSVar* ls_predec(LSVar*);
	static LSVar* ls_postdec(LSVar*);
	static LSVar* ls_abso(LSVar*);

	static LSVar* ls_add(LSVar*, LSVar*);
	static LSVar* ls_add_eq(LSVar*, LSVar*);
	static LSVar* ls_sub(LSVar*, LSVar*);
	static LSVar* ls_sub_eq(LSVar*, LSVar*);
	static LSVar* ls_mul(LSVar*, LSVar*);
	static LSVar* ls_mul_eq(LSVar*, LSVar*);
	static LSVar* ls_div(LSVar*, LSVar*);
	static LSVar* ls_div_eq(LSVar*, LSVar*);
	static LSVar* ls_pow(LSVar*, LSVar*);
	static LSVar* ls_pow_eq(LSVar*, LSVar*);
	static LSVar* ls_mod(LSVar*, LSVar*);
	static LSVar* ls_mod_eq(LSVar*, LSVar*);
};

}
#endif // LSVAR_H
