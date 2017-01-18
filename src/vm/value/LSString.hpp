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

	struct iterator {
		char* buffer;
		int index;
		int next_index;
		u_int32_t character;
	};

	static LSValue* string_class;
	static u_int32_t u8_char_at(char* s, int pos);
	static iterator iterator_begin(LSString* s);
	static void iterator_next(iterator* it);
	static u_int32_t iterator_get(iterator* it);
	static bool iterator_end(iterator* it);

	LSString();
	LSString(char);
	LSString(const char*);
	LSString(const std::string&);
	LSString(const Json&);

	virtual ~LSString();

	LSString* charAt(int index) const;
	int unicode_length() const;
	bool is_permutation(LSString* other);
	LSString* sort();
	bool is_palindrome() const;
	LSValue* ls_foldLeft(LSFunction<LSValue*>*, LSValue* v0);

	/*
	 * LSValue methods
	 */
	LSVALUE_OPERATORS
	bool isTrue() const override;

	LSValue* ls_not() override;
	LSValue* ls_tilde() override;

	LSValue* ls_add(LSNull*) override;
	LSValue* ls_add(LSBoolean*) override;
	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString*) override;
	LSValue* ls_add(LSArray<LSValue*>*) override;
	LSValue* ls_add(LSArray<int>*) override;
	LSValue* ls_add(LSObject*) override;
	LSValue* ls_add(LSFunction<LSValue*>*) override;
	LSValue* ls_add(LSClass*) override;

	LSValue* ls_add_eq(LSNull*) override;
	LSValue* ls_add_eq(LSBoolean*) override;
	LSValue* ls_add_eq(LSNumber*) override;
	LSValue* ls_add_eq(LSString*) override;
	LSValue* ls_add_eq(LSArray<LSValue*>*) override;
	LSValue* ls_add_eq(LSArray<int>*) override;
	LSValue* ls_add_eq(LSObject*) override;
	LSValue* ls_add_eq(LSFunction<LSValue*>*) override;
	LSValue* ls_add_eq(LSClass*) override;

	LSValue* ls_mul(LSNumber*) override;
	LSValue* ls_div(LSString*) override;

	bool eq(const LSString*) const override;
	bool lt(const LSString*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* range(int start, int end) const override;
	LSValue* rangeL(int start, int end) override;

	LSValue* abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const;
	std::ostream& dump(std::ostream& os) const;
	std::string json() const override;
	std::string escaped(char quote) const;
	std::string escape_control_characters() const;

	LSValue* getClass() const override;
	int typeID() const override { return 4; }
};

}

#endif
