#include "Match.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"

using namespace std;

namespace ls {

Match::Match() {
	value = nullptr;
}

Match::~Match() {
	delete value;
	for (auto& ps : pattern_list) {
		for (Pattern p : ps) {
			delete p.begin;
			delete p.end;
		}
	}
	for (auto x : returns) {
		delete x;
	}
}

void Match::print(std::ostream& os, int indent, bool debug) const {
	os << "match ";
	value->print(os, indent, debug);
	os << " {";
	for (size_t i = 0; i < pattern_list.size(); ++i) {

		os << endl << tabs(indent + 1);

		const vector<Pattern>& list = pattern_list[i];
		for (size_t j = 0; j < list.size(); ++j) {
			if (j > 0) {
				os << "|";
			}
			list[j].print(os, indent + 1, debug);
		}
		os << " : ";
		returns[i]->print(os, indent + 1, debug);
	}
	os << endl << tabs(indent) << "}";
	if (debug) {
		os << " " << type;
	}
}

unsigned Match::line() const {
	return 0;
}

void Match::preanalyse(SemanticAnalyser* analyser)
{
	// TODO
	assert(0);
}

void Match::will_require(SemanticAnalyser* analyser, const Type& req_type)
{

}

void Match::analyse(ls::SemanticAnalyser* analyser, const Type& req_type)
{
	assert(0);
	value->analyse(analyser, Type::UNKNOWN);
	if (value->type == Type::FUNCTION || value->type == Type::VOID) {
		stringstream oss;
		value->print(oss);
		analyser->add_error({ SemanticException::TYPE_MISMATCH, value->line(), oss.str() });
	}

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) {
				p.begin->analyse(analyser, Type::UNKNOWN);
			}
			if (p.end) {
				p.end->analyse(analyser, Type::UNKNOWN);
			}
		}
	}

	type = req_type;
	for (Value* ret : returns) {
		ret->preanalyse(analyser);
		if (!Type::intersection(type, ret->type, &type)) {
			stringstream oss;
			ret->print(oss);
			analyser->add_error({ SemanticException::INCOMPATIBLE_TYPES, ret->line(), oss.str() });
			break;
		}
	}
	type.make_it_pure();

	for (Value* ret : returns) {
		ret->analyse(analyser, type);
	}
}

/*
 * create res
 *
 * if not pattern[0]==value goto next[0]
 * res = return[0]
 * goto end
 * next[0]:
 *
 * if not pattern[1]==value goto next[1]
 * res = return[1]
 * goto end
 * next[1]:
 *
 * if not pattern[2]==value goto next[2]
 * res = return[2]
 * goto end
 * next[2]:
 *
 * res = default
 *
 * end:
 * return res
 */

jit_value_t Match::compile(Compiler& c) const {

	jit_value_t v = value->compile(c);

	jit_value_t res = jit_value_create(c.F, type.jit_type());
	jit_label_t label_end = jit_label_undefined;

	for (size_t i = 0; i < pattern_list.size(); ++i) {

		bool is_default = false;
		for (const Pattern& pattern : pattern_list[i]) {
			is_default = is_default || pattern.is_default();
		}

		if (is_default) {
			jit_value_t ret = returns[i]->compile(c);
			jit_insn_store(c.F, res, ret);
			jit_insn_label(c.F, &label_end);
			if (value->type.must_manage_memory()) {
				VM::delete_temporary(c.F, v);
			}
			return res;
		}

		jit_label_t label_next = jit_label_undefined;

		if (pattern_list[i].size() == 1) {
			jit_value_t cond = pattern_list[i][0].match(c, v, value->type);
			jit_insn_branch_if_not(c.F, cond, &label_next);
		} else {
			jit_label_t label_match = jit_label_undefined;

			for (const Pattern& pattern : pattern_list[i]) {
				jit_value_t cond = pattern.match(c, v, value->type);
				jit_insn_branch_if(c.F, cond, &label_match);
			}
			jit_insn_branch(c.F, &label_next);
			jit_insn_label(c.F, &label_match);
		}

		jit_value_t ret = returns[i]->compile(c);
		jit_insn_store(c.F, res, ret);
		jit_insn_branch(c.F, &label_end);
		jit_insn_label(c.F, &label_next);
	}
	// In the case of no default pattern

	jit_insn_store(c.F, res, VM::create_default(c.F, type));

	jit_insn_label(c.F, &label_end);
	if (value->type.must_manage_memory()) {
		VM::delete_temporary(c.F, v);
	}
	return res;
}

Match::Pattern::Pattern(Value* value)
	: interval(false), begin(value), end(nullptr) {}

Match::Pattern::Pattern(Value* begin, Value* end)
	: interval(true), begin(begin), end(end) {}

Match::Pattern::~Pattern() {}

void Match::Pattern::print(ostream &os, int indent, bool debug) const {
	if (interval) {
		if (begin) begin->print(os, indent, debug);
		os << "..";
		if (end) end->print(os, indent, debug);
	} else {
		begin->print(os, indent, debug);
	}
}

jit_value_t Match::Pattern::match(Compiler &c, jit_value_t v, const Type& value_type) const {

	if (interval) {
		jit_value_t ge = nullptr;
		if (begin) {
			jit_value_t b = begin->compile(c);
			ge = Compiler::compile_ge(c.F, v, value_type, b, begin->type);
			if (begin->type.must_manage_memory()) {
				VM::delete_temporary(c.F, b);
			}
		}

		jit_value_t lt = nullptr;
		if (end) {
			jit_value_t e = end->compile(c);
			lt = Compiler::compile_lt(c.F, v, value_type, e, end->type);
			if (end->type.must_manage_memory()) {
				VM::delete_temporary(c.F, e);
			}
		}

		if (ge) {
			if (lt) {
				return jit_insn_and(c.F, ge, lt);
			} else {
				return ge;
			}
		} else {
			if (lt) {
				return lt;
			} else {
				return nullptr; // équivalent à default
			}
		}

	} else {
		jit_value_t p = begin->compile(c);
		jit_value_t cond = Compiler::compile_eq(c.F, v, value_type, p, begin->type);
		if (begin->type.must_manage_memory()) {
			VM::delete_temporary(c.F, p);
		}
		return cond;
	}
}

}
