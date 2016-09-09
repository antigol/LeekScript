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

// DONE 2
void Match::analyse_help(SemanticAnalyser* analyser)
{
	value->analyse(analyser);

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) p.begin->analyse(analyser);
			if (p.end)   p.end->analyse(analyser);
		}
	}

	for (Value* ret : returns) {
		ret->analyse(analyser);
	}

	type = Type::UNKNOWN;
}

void Match::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	Type old_type;
	Type new_type = value->type;
	do {
		old_type = new_type;

		value->reanalyse(analyser, new_type);
		if (!Type::intersection(new_type, value->type, &new_type)) {
			add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
		}

		for (auto& ps : pattern_list) {
			for (Pattern& p : ps) {
				if (p.begin) {
					p.begin->reanalyse(analyser, new_type);
					if (!Type::intersection(new_type, p.begin->type, &new_type)) {
						add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
					}
				}
				if (p.end) {
					p.end->reanalyse(analyser, new_type);
					if (!Type::intersection(new_type, p.end->type, &new_type)) {
						add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
					}
				}
			}
		}
	} while (new_type != old_type && analyser->errors.empty());

	Type ret_type = type;
	do {
		type = ret_type;

		for (Value* ret : returns) {
			ret->reanalyse(analyser, ret_type);
			if (!Type::intersection(ret_type, ret->type, &ret_type)) {
				add_error(analyser, SemanticException::INCOMPATIBLE_TYPES);
			}
		}
	} while (ret_type != type && analyser->errors.empty());
}

void Match::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	if (!Type::intersection(type, req_type, &type)) {
		add_error(analyser, SemanticException::TYPE_MISMATCH);
	}

	value->finalize(analyser, Type::UNKNOWN);

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) p.begin->finalize(analyser, value->type);
			if (p.end)   p.end->finalize(analyser, value->type);
		}
	}

	for (Value* ret : returns) {
		ret->finalize(analyser, type);
		type = ret->type;
	}
	type.make_it_pure(); // empty match

	assert(type.is_pure() || !analyser->errors.empty());
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
			Compiler::compile_delete_temporary(c.F, v, value->type);
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
	Compiler::compile_delete_temporary(c.F, v, value->type);
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
			Compiler::compile_delete_temporary(c.F, b, begin->type);
		}

		jit_value_t lt = nullptr;
		if (end) {
			jit_value_t e = end->compile(c);
			lt = Compiler::compile_lt(c.F, v, value_type, e, end->type);
			Compiler::compile_delete_temporary(c.F, e, end->type);
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
		Compiler::compile_delete_temporary(c.F, p, begin->type);
		return cond;
	}
}

}
