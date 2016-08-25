#ifndef LSSTRING_H_
#define LSSTRING_H_

#include <iostream>
#include <string>

#include "../LSValue.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSString : public LSValue, public std::string {
public:

	static LSValue* string_class;
	static u_int32_t u8_char_at(char* s, int pos);

	LSString();
	LSString(char);
	LSString(const char*);
	LSString(const std::string&);
	LSString(const Json&);

	virtual ~LSString();

	LSString* charAt(int index) const;
	int unicode_length() const;

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSValue* ls_not() override;
	LSValue* ls_tilde() override;

	LSVALUE_OPERATORS

	LSValue* ls_add(LSNull*) override;
	LSValue* ls_add(LSBoolean*) override;
	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString*) override;
	LSValue* ls_add(LSArray<LSValue*>*) override;
	LSValue* ls_add(LSArray<int>*) override;
	LSValue* ls_add(LSObject*) override;
	LSValue* ls_add(LSFunction*) override;
	LSValue* ls_add(LSClass*) override;

	LSValue* ls_add_eq(LSNull*) override;
	LSValue* ls_add_eq(LSBoolean*) override;
	LSValue* ls_add_eq(LSNumber*) override;
	LSValue* ls_add_eq(LSString*) override;
	LSValue* ls_add_eq(LSArray<LSValue*>*) override;
	LSValue* ls_add_eq(LSArray<int>*) override;
	LSValue* ls_add_eq(LSObject*) override;
	LSValue* ls_add_eq(LSFunction*) override;
	LSValue* ls_add_eq(LSClass*) override;

	LSValue* ls_mul(LSNumber*) override;
	LSValue* ls_div(LSString*) override;

	bool eq(const LSString*) const override;
	bool lt(const LSString*) const override;


	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* range(int start, int end) const override;
	LSValue* rangeL(int start, int end) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const;
	std::string json() const override;
	std::string escaped(char quote) const;

	LSValue* getClass() const override;

	int typeID() const override { return 4; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
