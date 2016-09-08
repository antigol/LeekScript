#include "Number.hpp"
#include "../../vm/VM.hpp"
#include <climits>
#include <cassert>

using namespace std;

double stod_(string str) {

	if (str.size() > 2 && str[0] == '0' && str[1] == 'b') {
		double x = 0.0;
		for (size_t i = 2; i < str.size(); ++i) {
			x *= 2.0;
			if (str[i] == '1') x += 1.0;
		}
		return x;
	} else {
		return stod(str);
	}
}

namespace ls {

Number::Number(const string& value, Token* token) {
	this->value = value;
	this->token = token;
}

Number::~Number() {
}

void Number::print(ostream& os, int, bool debug) const {
	os << value;
	if (debug) {
		os << " " << type;
	}
}

unsigned Number::line() const {
	return token->line;
}

void Number::analyse_help(SemanticAnalyser* analyser)
{
	constant = true;
	if (value.find('.') != string::npos) {
		type = Type({ Type::F64, Type::VAR });
	} else {
		type = Type({ Type::I32, Type::F64, Type::VAR });
	}
}

void Number::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
}

void Number::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}
	type.make_it_pure();
}


jit_value_t Number::compile(Compiler& c) const
{
	double v = stod_(value);

	if (type == Type::VAR) {
		return VM::create_lsreal(c.F, v);
	}
	if (type == Type::F32) {
		return VM::create_f32(c.F, v);
	}
	if (type == Type::F64) {
		return VM::create_f64(c.F, v);
	}
	if (type == Type::I32) {
		return VM::create_i32(c.F, v);
	}
	if (type == Type::I64) {
		return VM::create_i64(c.F, v);
	}
	assert(0);
	return nullptr;
}

}
