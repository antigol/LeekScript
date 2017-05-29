#include <sstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "../../../lib/utf8.h"

#include "StringSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSArray.hpp"

using namespace std;

namespace ls {

LSValue* string_charAt(LSString* string, int index);
bool string_contains(LSString* haystack, LSString* needle);
bool string_endsWith(LSString* string, LSString* ending);
int string_indexOf(LSString* haystack, LSString* needle);
int string_length(LSString* string);
LSString* string_map(LSString* string, LSFunction<LSValue*>* fun);
LSValue* string_size_ptr(LSString* string);
int string_size(LSString* string);
LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter);
bool string_startsWith(const LSString* string, const LSString* starting);
LSValue* string_substring(LSString* string, int start, int length);
LSValue* string_toLower(LSString* string);
LSValue* string_toUpper(LSString* string);
LSValue* string_toArray(const LSString* string);
int string_begin_code(const LSString*);
LSValue* string_begin_code_ptr(const LSString*);
int string_code(const LSString*, int pos);
long string_number(const LSString*);

LSString* plus_mpz(LSString* s, __mpz_struct mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, &mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	return res;
}

LSString* plus_mpz_tmp(LSString* s, __mpz_struct mpz) {
	char buff[1000];
	mpz_get_str(buff, 10, &mpz);
	LSString* res = new LSString(*s + buff);
	LSValue::delete_temporary(s);
	mpz_clear(&mpz);
	VM::current()->mpz_deleted++;
	return res;
}

StringSTD::StringSTD() : Module("String") {

	LSString::string_class = clazz;

	/*
	 * Operators
	 */
	operator_("+", {
		{Type::STRING, Type::INTEGER, Type::STRING, (void*) &StringSTD::add_int, Method::NATIVE},
		{Type::STRING, Type::REAL, Type::STRING, (void*) &StringSTD::add_real, Method::NATIVE},
		{Type::STRING, Type::MPZ, Type::STRING, (void*) &plus_mpz, Method::NATIVE},
		{Type::STRING, Type::MPZ_TMP, Type::STRING, (void*) &plus_mpz_tmp, Method::NATIVE}
	});
	operator_("<", {
		{Type::STRING, Type::STRING, Type::BOOLEAN, (void*) &StringSTD::lt}
	});

	/*
	 * Methods
	 */
	method("charAt", {
		{Type::STRING, Type::STRING, {Type::INTEGER}, (void*) &LSString::charAt, Method::NATIVE}
	});
	method("contains", {
		{Type::STRING, Type::BOOLEAN, {Type::STRING}, (void*) &string_contains, Method::NATIVE}
	});
	method("endsWith", {
		{Type::STRING, Type::BOOLEAN, {Type::STRING}, (void*) &string_endsWith, Method::NATIVE}
	});
	Type fold_fun_type = Type::FUNCTION_P;
	fold_fun_type.setArgumentType(0, Type::POINTER);
	fold_fun_type.setArgumentType(1, Type::STRING);
	fold_fun_type.setReturnType(Type::POINTER);
	method("fold", {
		{Type::STRING, Type::POINTER, {fold_fun_type, Type::POINTER}, (void*) &LSString::ls_foldLeft, Method::NATIVE}
	});
	method("indexOf", {
		{Type::STRING, Type::INTEGER, {Type::STRING}, (void*) &string_indexOf, Method::NATIVE}
	});
	method("isPermutation", {
		{Type::STRING, Type::BOOLEAN, {Type::POINTER}, (void*) &LSString::is_permutation, Method::NATIVE}
	});
	method("isPalindrome", {
		{Type::STRING, Type::BOOLEAN, {}, (void*) &LSString::is_palindrome, Method::NATIVE}
	});
	method("length", {
		{Type::STRING, Type::INTEGER, {}, (void*) &string_length, Method::NATIVE}
	});
	method("size", {
		{Type::STRING, Type::INTEGER, {}, (void*) &string_size, Method::NATIVE}
	});
	method("replace", {
		{Type::STRING, Type::STRING, {Type::STRING, Type::STRING}, (void*) &StringSTD::replace, Method::NATIVE}
	});
	method("reverse", {
		{Type::STRING, Type::STRING, {}, (void*) &LSString::ls_tilde, Method::NATIVE}
	});
	method("substring", {
		{Type::STRING, Type::STRING, {Type::INTEGER, Type::INTEGER}, (void*) &string_substring, Method::NATIVE}
	});
	method("toArray", {
		{Type::STRING, Type::PTR_ARRAY, {}, (void*) &string_toArray, Method::NATIVE}
	});
	method("toLower", {
		{Type::STRING, Type::STRING, {}, (void*) &string_toLower, Method::NATIVE}
	});
	method("toUpper", {
		{Type::STRING, Type::STRING, {}, (void*) &string_toUpper, Method::NATIVE}
	});
	method("split", {
		{Type::STRING, Type::STRING_ARRAY, {Type::STRING}, (void*) &string_split, Method::NATIVE}
	});
	method("startsWith", {
		{Type::STRING, Type::BOOLEAN, {Type::STRING}, (void*) &string_startsWith, Method::NATIVE}
	});
	method("code", {
		{Type::STRING, Type::INTEGER, {}, (void*) &string_begin_code, Method::NATIVE},
		{Type::STRING, Type::INTEGER, {Type::INTEGER}, (void*) &string_code, Method::NATIVE},
	});
	method("number", {
		{Type::STRING, Type::LONG, {}, (void*) &string_number, Method::NATIVE}
	});
	Type map_fun_type = Type::FUNCTION_P;
	map_fun_type.setArgumentType(0, Type::STRING);
	map_fun_type.setReturnType(Type::STRING);
	method("map", {
		{Type::STRING, Type::STRING, {map_fun_type}, (void*) &string_map, Method::NATIVE}
	});
	method("sort", {
		{Type::STRING, Type::STRING, {}, (void*) &LSString::sort, Method::NATIVE}
	});

	/*
	 * Static methods
	 */
	static_method("charAt", {
		{Type::STRING, {Type::STRING, Type::INTEGER}, (void*) &string_charAt, Method::NATIVE}
	});
	static_method("contains", {
		{Type::BOOLEAN, {Type::STRING, Type::STRING}, (void*) &string_contains, Method::NATIVE}
	});
	static_method("endsWith", {
		{Type::BOOLEAN, {Type::STRING, Type::STRING}, (void*) &string_endsWith, Method::NATIVE}
	});
	static_method("indexOf", {
		{Type::INTEGER, {Type::STRING, Type::STRING}, (void*) &string_indexOf, Method::NATIVE}
	});
	static_method("length", {
		{Type::INTEGER, {Type::STRING}, (void*) &string_length, Method::NATIVE}
	});
	static_method("size", {
		{Type::NUMBER, {Type::STRING}, (void*) &string_size_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::STRING}, (void*) &string_size, Method::NATIVE}
	});
	static_method("replace", {
		{Type::STRING, {Type::STRING, Type::STRING, Type::STRING}, (void*) &StringSTD::replace, Method::NATIVE}
	});
	static_method("reverse", {
		{Type::STRING, {Type::STRING}, (void*) &LSString::ls_tilde, Method::NATIVE}
	});
	static_method("substring", {
		{Type::STRING, {Type::STRING, Type::INTEGER, Type::INTEGER}, (void*) &string_substring, Method::NATIVE}
	});
	static_method("toArray", {
		{Type::PTR_ARRAY, {Type::STRING}, (void*) &string_toArray, Method::NATIVE}
	});
	static_method("toLower", {
		{Type::STRING, {Type::STRING}, (void*) &string_toLower, Method::NATIVE}
	});
	static_method("toUpper", {
		{Type::STRING, {Type::STRING}, (void*) &string_toUpper, Method::NATIVE}
	});
	static_method("split", {
		{Type::STRING_ARRAY, {Type::POINTER, Type::POINTER}, (void*) &string_split, Method::NATIVE}
	});
	static_method("startsWith", {
		{Type::BOOLEAN, {Type::STRING, Type::STRING}, (void*) &string_startsWith, Method::NATIVE}
	});
	static_method("map", {
		{Type::STRING, {Type::STRING, map_fun_type}, (void*) &string_map, Method::NATIVE}
	});
	static_method("code", {
		{Type::POINTER, {Type::POINTER}, (void*) &string_begin_code_ptr, Method::NATIVE},
		{Type::INTEGER, {Type::POINTER}, (void*) &string_begin_code, Method::NATIVE},
		{Type::INTEGER, {Type::STRING, Type::INTEGER}, (void*) &string_code, Method::NATIVE},
	});
	static_method("number", {
		{Type::LONG, {Type::POINTER}, (void*) &string_number, Method::NATIVE}
	});
}

StringSTD::~StringSTD() {}

/*
 * Operators
 */
LSString* StringSTD::add_int(LSString* s, int i) {
	if (s->refs == 0) {
		s->append(std::to_string(i));
		return s;
	} else {
		return new LSString(*s + std::to_string(i));
	}
}

LSString* StringSTD::add_real(LSString* s, double i) {
	if (s->refs == 0) {
		s->append(LSNumber::print(i));
		return s;
	} else {
		return new LSString(*s + LSNumber::print(i));
	}
}

Compiler::value StringSTD::lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
		return a->lt(b);
	});
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

/*
 * Methods
 */
LSValue* string_charAt(LSString* string, int index) {
	LSValue* r = new LSString(string->operator[] (index));
	LSValue::delete_temporary(string);
	return r;
}

bool string_contains(LSString* haystack, LSString* needle) {
	bool r = haystack->find(*needle) != string::npos;
	LSValue::delete_temporary(haystack);
	LSValue::delete_temporary(needle);
	return r;
}

bool string_endsWith(LSString* string, LSString* ending) {
	if (ending->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(ending);
		return false;
	}
	bool r = std::equal(ending->rbegin(), ending->rend(), string->rbegin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(ending);
	return r;
}

int string_indexOf(LSString* string, LSString* needle) {
	int pos = -1;
	if (needle->size() <= string->size()) {
		pos = string->find(*needle);
	}
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(needle);
	return pos;
}

int string_length(LSString* string) {
	int r = string->size();
	LSValue::delete_temporary(string);
	return r;
}

LSString* string_map(LSString* s, LSFunction<LSValue*>* function) {

	char buff[5];
	LSValue* r = new LSString();
	auto fun = (LSValue* (*)(void*, void*)) function->function;

	const char* string_chars = s->c_str();
	int i = 0;
	int l = strlen(string_chars);

	while (i < l) {
		u_int32_t c = u8_nextchar(string_chars, &i);
		u8_toutf8(buff, 5, &c, 1);
		LSString* ch = new LSString(buff);
		ch->refs = 1;
		r->add_eq(fun(function, ch));
		LSValue::delete_ref(ch);
	}
	LSValue::delete_temporary(s);
	return (LSString*) r;
}

LSString* StringSTD::replace(LSString* string, LSString* from, LSString* to) {
	std::string str(*string);
	size_t start_pos = 0;
	while((start_pos = str.find(*from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from->length(), *to);
		start_pos += to->length();
	}
	if (string->refs == 0) {
		delete string;
	}
	if (from->refs == 0) {
		delete from;
	}
	if (to->refs == 0) {
		delete to;
	}
	return new LSString(str);
}

int string_size(LSString* string) {
	int r = string->unicode_length();
	LSValue::delete_temporary(string);
	return r;
}

LSValue* string_size_ptr(LSString* string) {
	return LSNumber::get(string_size(string));
}

LSArray<LSValue*>* string_split(LSString* string, LSString* delimiter) {
	auto parts = new LSArray<LSValue*>();
	if (*delimiter == "") {
		for (char c : *string) {
			parts->push_inc(new LSString(c));
		}
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	} else {
		size_t last = 0;
		size_t pos = 0;
		while ((pos = string->find(*delimiter, last)) != std::wstring::npos) {
			parts->push_inc(new LSString(string->substr(last, pos - last)));
			last = pos + delimiter->size();
		}
		parts->push_inc(new LSString(string->substr(last)));
		if (string->refs == 0) {
			delete string;
		}
		if (delimiter->refs == 0) {
			delete delimiter;
		}
		return parts;
	}
}

bool string_startsWith(const LSString* string, const LSString* starting) {
	if (starting->size() > string->size()) {
		LSValue::delete_temporary(string);
		LSValue::delete_temporary(starting);
		return false;
	}
	bool r = std::equal(starting->begin(), starting->end(), string->begin());
	LSValue::delete_temporary(string);
	LSValue::delete_temporary(starting);
	return r;
}

LSValue* string_substring(LSString* string, int start, int length) {
	LSValue* r = new LSString(string->substr(start, length));
	if (string->refs == 0) {
		delete string;
	}
	return r;
}

LSValue* string_toArray(const LSString* string) {
	LSArray<LSValue*>* parts = new LSArray<LSValue*>();
	for (char c : *string) {
		parts->push_inc(new LSString(c));
	}
	if (string->refs == 0) {
		delete string;
	}
	return parts;
}

LSValue* string_toLower(LSString* s) {
	string new_s = string(*s);
	for (auto& c : new_s) c = tolower(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

LSValue* string_toUpper(LSString* s) {
	string new_s = string(*s);
	for (auto& c : new_s) c = toupper(c);
	if (s->refs == 0) {
		delete s;
	}
	return new LSString(new_s);
}

int string_begin_code(const LSString* v) {
	int r = LSString::u8_char_at((char*) v->c_str(), 0);
	LSValue::delete_temporary(v);
	return r;
}

LSValue* string_begin_code_ptr(const LSString* v) {
	auto code = string_begin_code(v);
	auto r = LSNumber::get(code);
	return r;
}

int string_code(const LSString* v, int pos) {
	int r = LSString::u8_char_at((char*) v->c_str(), pos);
	LSValue::delete_temporary(v);
	return r;
}

long string_number(const LSString* s) {
	long r = stol(*s);
	if (s->refs == 0) delete s;
	return r;
}

}
