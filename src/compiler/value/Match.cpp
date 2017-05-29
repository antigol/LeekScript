#include "Match.hpp"
#include "../../vm/LSValue.hpp"
#include "../../vm/VM.hpp"

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

Location Match::location() const {
	return {{0, 0, 0}, {0, 0, 0}}; // TODO
}

void Match::analyse(ls::SemanticAnalyser* analyser, const Type&) {

	bool any_pointer = false;
	bool has_default = false;

	value->analyse(analyser, Type::UNKNOWN);
	if (value->type.nature == Nature::POINTER) any_pointer = true;

	for (auto& ps : pattern_list) {
		for (Pattern& p : ps) {
			if (p.begin) {
				p.begin->analyse(analyser, Type::UNKNOWN);
				if (p.begin->type.nature == Nature::POINTER) any_pointer = true;
			}
			if (p.end) {
				p.end->analyse(analyser, Type::UNKNOWN);
				if (p.end->type.nature == Nature::POINTER) any_pointer = true;
			}
			has_default = has_default || p.is_default();
		}
	}

	if (any_pointer) {
		value->analyse(analyser, Type::POINTER);
		for (auto& ps : pattern_list) {
			for (Pattern& p : ps) {
				if (p.begin) {
					p.begin->analyse(analyser, Type::POINTER);
				}
				if (p.end) {
					p.end->analyse(analyser, Type::POINTER);
				}
			}
		}
	}

	if (!has_default) {
		// Return type is always pointer because in the default case, null is return
		type = Type::POINTER;
		for (Value* r : returns) {
			r->analyse(analyser, Type::POINTER);
		}
	} else {
		type = Type::UNKNOWN;
		for (Value* ret : returns) {
			ret->analyse(analyser, Type::UNKNOWN);
			type = Type::get_compatible_type(type, ret->type);
		}
		for (Value* ret : returns) {
			ret->analyse(analyser, type);
		}
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

Compiler::value Match::compile(Compiler& c) const {

	auto v = value->compile(c);

	jit_value_t res = jit_value_create(c.F, VM::get_jit_type(type));
	jit_label_t label_end = jit_label_undefined;

	for (size_t i = 0; i < pattern_list.size(); ++i) {

		bool is_default = false;
		for (const Pattern& pattern : pattern_list[i]) {
			is_default = is_default || pattern.is_default();
		}

		if (is_default) {
			auto ret = returns[i]->compile(c);
			jit_insn_store(c.F, res, ret.v);
			jit_insn_label(c.F, &label_end);
			c.insn_delete_temporary(v);
			return {res, type};
		}

		jit_label_t label_next = jit_label_undefined;

		if (pattern_list[i].size() == 1) {
			jit_value_t cond = pattern_list[i][0].match(c, v.v);
			jit_insn_branch_if_not(c.F, cond, &label_next);
		} else {
			jit_label_t label_match = jit_label_undefined;

			for (const Pattern& pattern : pattern_list[i]) {
				jit_value_t cond = pattern.match(c, v.v);
				jit_insn_branch_if(c.F, cond, &label_match);
			}
			jit_insn_branch(c.F, &label_next);
			jit_insn_label(c.F, &label_match);
		}

		auto ret = returns[i]->compile(c);
		jit_insn_store(c.F, res, ret.v);
		jit_insn_branch(c.F, &label_end);
		jit_insn_label(c.F, &label_next);
	}
	// In the case of no default pattern

	jit_insn_store(c.F, res, c.new_null().v);

	jit_insn_label(c.F, &label_end);
	c.insn_delete_temporary(v);
	return {res, type};
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

bool jit_equals_(LSValue* x, LSValue* y) {
	return *x == *y;
}
bool jit_less_(LSValue* x, LSValue* y) {
	return *x < *y;
}
bool jit_greater_equal_(LSValue* x, LSValue* y) {
	return *x >= *y;
}

jit_value_t Match::Pattern::match(Compiler &c, jit_value_t v) const {

	jit_type_t args_types[2] = {LS_POINTER, LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_bool, args_types, 2, 1);

	if (interval) {
		jit_value_t ge = nullptr;
		if (begin) {
			auto b = begin->compile(c);
			if (begin->type.nature == Nature::VALUE) {
				ge = jit_insn_ge(c.F, v, b.v);
			} else {
				jit_value_t args[2] = { v, b.v };
				ge = jit_insn_call_native(c.F, "", (void*) jit_greater_equal_, sig, args, 2, JIT_CALL_NOTHROW);
				c.insn_delete_temporary(b);
			}
		}
		jit_value_t lt = nullptr;
		if (end) {
			auto e = end->compile(c);
			if (end->type.nature == Nature::VALUE) {
				lt = jit_insn_lt(c.F, v, e.v);
			} else {
				jit_value_t args[2] = { v, e.v };
				lt = jit_insn_call_native(c.F, "", (void*) jit_less_, sig, args, 2, JIT_CALL_NOTHROW);
				c.insn_delete_temporary(e);
			}
		}
		jit_type_free(sig);
		if (ge) {
			if (lt) {
				return jit_insn_and(c.F, ge, lt);
			} else {
				return ge;
			}
		} else {
			return lt;
		}
	} else {
		jit_value_t cond;
		auto p = begin->compile(c);

		if (begin->type.nature == Nature::VALUE) {
			cond = jit_insn_eq(c.F, v, p.v);
		} else {
			jit_value_t args[2] = { v, p.v };
			cond = jit_insn_call_native(c.F, "", (void*) jit_equals_, sig, args, 2, JIT_CALL_NOTHROW);
			c.insn_delete_temporary(p);
		}
		jit_type_free(sig);
		return cond;
	}
}

Value* Match::clone() const {
	auto match = new Match();
	match->value = value->clone();
	for (const auto& pl : pattern_list) {
		match->pattern_list.push_back({});
		for (const auto& p : pl) {
			match->pattern_list.back().push_back(p.clone());
		}
	}
	return match;
}

}
