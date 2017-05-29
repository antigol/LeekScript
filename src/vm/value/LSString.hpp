#ifndef LSSTRING_H_
#define LSSTRING_H_

#include <iostream>
#include <string>
#include "../LSValue.hpp"

namespace ls {

class LSString : public LSValue, public std::string {
public:

	struct iterator {
		char* buffer;
		int index;
		int pos;
		int next_pos;
		u_int32_t character;
	};

	static LSValue* string_class;
	static u_int32_t u8_char_at(char* s, int pos);
	static iterator iterator_begin(LSString* s);
	static void iterator_next(iterator* it);
	static u_int32_t iterator_get(iterator* it);
	static int iterator_key(LSString::iterator* it);
	static bool iterator_end(iterator* it);

	LSString();
	LSString(char);
	LSString(const char*);
	LSString(const std::string&);
	LSString(const Json&);

	virtual ~LSString();

	LSString* charAt(int index) const;
	LSString* codePointAt(int index) const;
	int unicode_length() const;
	bool is_permutation(LSString* other);
	LSString* sort();
	bool is_palindrome() const;
	LSValue* ls_foldLeft(LSFunction<LSValue*>*, LSValue* v0);
	int int_size() const;

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
 	LSValue* ls_tilde() override;

	virtual LSValue* add(LSValue* v) override;
	virtual LSValue* add_eq(LSValue* v) override;
	virtual LSValue* mul(LSValue* v) override;
	virtual LSValue* div(LSValue* v) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	LSValue* at (const LSValue* value) const override;

	LSValue* range(int start, int end) const override;

	int abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const override;
	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;
	std::string escaped(char quote) const;
	std::string escape_control_characters() const;

	LSValue* getClass() const override;
};

}

#endif
