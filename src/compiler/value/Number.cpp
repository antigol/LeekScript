#include "Number.hpp"

#include "../../vm/VM.hpp"
#include <limits.h>
#include "../../vm/value/LSNumber.hpp"

using namespace std;

namespace ls {

Number::Number(std::string value, std::shared_ptr<Token> token) : value(value), token(token) {
	constant = true;
}

Number::~Number() {
	if (mpz_value_initialized) {
		mpz_clear(mpz_value);
	}
}

void Number::print(ostream& os, int, bool debug) const {
	os << value;
	if (debug) {
		os << " " << types;
	}
}

Location Number::location() const {
	return token->location;
}

void Number::analyse(SemanticAnalyser*, const Type& req_type) {

	// Get the base
	base = 10;
	clean_value = value;
	if (value.size() > 2 and value[0] == '0') {
		if (value[1] == 'b') {
			base = 2;
			clean_value = value.substr(2);
		} else if (value[1] == 'x') {
			base = 16;
			clean_value = value.substr(2);
		}
	}
	// long and MP number?
	bool long_number = clean_value[clean_value.size() - 1] == 'l';
	bool mp_number = clean_value[clean_value.size() - 1] == 'm';
	if (long_number or mp_number) {
		clean_value = clean_value.substr(0, clean_value.size() - 1);
	}
	// Floating-point?
	bool floating = std::find(clean_value.begin(), clean_value.end(), '.') != clean_value.end() or req_type.raw_type == RawType::REAL;

	// Determine the possible container for the number
	if (floating) {
		if (!mp_number) {
			try {
				double_value = std::stod(clean_value);
			} catch (...) { // LCOV_EXCL_LINE
				mp_number = true; // number too large, GMP needed LCOV_EXCL_LINE
			}
		}
		if (mp_number) {
			// LCOV_EXCL_START
			if (!mpz_value_initialized) {
				mpf_init_set_str(mpf_value, clean_value.c_str(), base);
				mpz_value_initialized = true;
			}
			assert(false && "No support for mpf numbers yet");
			// LCOV_EXCL_STOP
		} else {
			type = Type::REAL;
		}
	} else {
		if (!mpz_value_initialized) {
			mpz_init_set_str(mpz_value, clean_value.c_str(), base);
			mpz_value_initialized = true;
		}
		if (!mp_number and !long_number and mpz_fits_sint_p(mpz_value)) {
			type = Type::INTEGER;
			int_value = mpz_get_si(mpz_value);
			double_value = int_value;
		} else if (!mp_number and mpz_fits_slong_p(mpz_value)) {
			type = Type::LONG;
			long_value = mpz_get_si(mpz_value);
			double_value = long_value;
		} else {
			type = Type::MPZ;
		}
	}

	if (pointer) {
		type.nature = Nature::POINTER;
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	types = type;
}

Compiler::value Number::compile(Compiler& c) const {

	if (type.nature == Nature::POINTER) {
		jit_value_t val = LS_CREATE_REAL(c.F, double_value);
		return {VM::value_to_pointer(c.F, val, Type::REAL), type};
	}
	if (type == Type::LONG) {
		return {LS_CREATE_LONG(c.F, long_value), type};
	}
	if (type == Type::REAL) {
		return {LS_CREATE_REAL(c.F, double_value), type};
	}

	if (type.raw_type == RawType::MPZ) {

		jit_value_t mpz_struct = jit_value_create(c.F, VM::mpz_type);
		jit_value_set_addressable(mpz_struct);
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, mpz_struct), 0, LS_CREATE_INTEGER(c.F, mpz_value->_mp_alloc));
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, mpz_struct), 4, LS_CREATE_INTEGER(c.F, mpz_value->_mp_size));
		jit_insn_store_relative(c.F, jit_insn_address_of(c.F, mpz_struct), 8, c.new_pointer(mpz_value->_mp_d).v);

		return {mpz_struct, Type::MPZ};
	}

	return {LS_CREATE_INTEGER(c.F, int_value), type};
}

Value* Number::clone() const {
	auto n = new Number(value, token);
	return n;
}

}
