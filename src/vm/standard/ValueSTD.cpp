#include "ValueSTD.hpp"
#include "JsonSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"
#include "../../vm/VM.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	/*
	 * Static attributes
	 */
	static_field("unknown", Type::UNKNOWN, (void*) &ValueSTD::unknown);

	/*
	 * Attributes
	 */
	field("class", Type::CLASS, (void*) &ValueSTD::attr_class);

	/*
	 * Operators
	 */
	operator_("=", {
 		{Type::BOOLEAN, Type::BOOLEAN, Type::BOOLEAN, (void*) &ValueSTD::op_store},
		//{Type::NUMBER_VALUE, Type::NUMBER_VALUE, Type::NUMBER_VALUE, (void*) &ValueSTD::op_store}
 	});
	operator_("is", {
		{Type::UNKNOWN, Type::CLASS, Type::BOOLEAN, (void*) &ValueSTD::op_instanceof}
	});
	operator_("==", {
		{Type::VALUE, Type::VALUE, Type::BOOLEAN, (void*) &ValueSTD::op_equals}
 	});
	operator_("!=", {
		{Type::VALUE, Type::VALUE, Type::BOOLEAN, (void*) &ValueSTD::op_not_equals}
 	});
	operator_("<", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_lt}
	});
	operator_("<=", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_le}
	});
	operator_(">", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_gt}
	});
	operator_(">=", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_ge}
	});
	operator_("and", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_and}
	});
	operator_("&&", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_and}
	});
	operator_("or", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_or}
	});
	operator_("xor", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_xor}
	});
	operator_("&", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::INTEGER, (void*) &ValueSTD::op_bit_and}
	});
	operator_("|", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::INTEGER, (void*) &ValueSTD::op_bit_or}
	});
	operator_("^", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::INTEGER, (void*) &ValueSTD::op_bit_xor}
	});
	operator_("in", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_in}
	});

	/*
	 * Methods
	 */
	method("string", Type::UNKNOWN, Type::STRING, {}, (void*) &ValueSTD::to_string);
	method("json", Type::UNKNOWN, Type::STRING, {}, (void*) &JsonSTD::encode);
	method("typeID", Type::UNKNOWN, Type::INTEGER, {}, (void*) &ValueSTD::typeID);
}

/*
 * Static attributes
 */
Compiler::value ValueSTD::unknown(Compiler& c) {
	return c.new_pointer(LSNumber::get(floor(1 + ((double) rand() / RAND_MAX) * 100)));
}

/*
 * Attributes
 */
Compiler::value ValueSTD::attr_class(Compiler& c, Compiler::value a) {
	auto clazz = c.insn_class_of(a);
	c.insn_delete_temporary(a);
	return clazz;
}

/*
 * Operators
 */
Compiler::value ValueSTD::op_store(Compiler& c, std::vector<Compiler::value> args) {
	c.insn_store_relative(args[0], 0, args[1]);
	return args[1];
}

Compiler::value ValueSTD::op_instanceof(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_eq(c.insn_class_of(args[0]), args[1]);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_eq(args[0], args[1]);
}

Compiler::value ValueSTD::op_not_equals(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_ne(args[0], args[1]);
}

Compiler::value ValueSTD::op_lt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0
		or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN,
			{c.insn_to_pointer(args[0]), c.insn_to_pointer(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a < *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_lt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_le(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0
		or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN,
			{c.insn_to_pointer(args[0]), c.insn_to_pointer(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a <= *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_le(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_gt(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0
		or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN,
			{c.insn_to_pointer(args[0]), c.insn_to_pointer(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a > *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_gt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_ge(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t.id() == args[1].t.id() or args[0].t.id() == 0
		or args[1].t.id() == 0) {
		auto res = c.insn_call(Type::BOOLEAN,
			{c.insn_to_pointer(args[0]), c.insn_to_pointer(args[1])},
			+[](LSValue* a, LSValue* b) {
				auto res = *a >= *b;
				LSValue::delete_temporary(a);
				LSValue::delete_temporary(b);
				return res;
			}
		);
		return res;
	} else {
		auto res = c.insn_ge(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return res;
	}
}

Compiler::value ValueSTD::op_and(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_and(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_or(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_or(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return res;
}

Compiler::value ValueSTD::op_xor(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_to_bool(args[0]);
	auto b = c.insn_to_bool(args[1]);
	auto r = c.insn_or(
		c.insn_and(a, c.insn_not(b)),
		c.insn_and(b, c.insn_not(a))
	);
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

Compiler::value ValueSTD::op_bit_and(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::INTEGER, {args[0], c.insn_to_pointer(args[1])},
	+[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
			(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
		}
		auto res = (int) a->value & (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_bit_or(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::INTEGER, {args[0], c.insn_to_pointer(args[1])},
	+[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
			(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
		}
		auto res = (int) a->value | (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_bit_xor(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::INTEGER, {args[0], c.insn_to_pointer(args[1])},
	+[](LSValue* x, LSValue* y) {
		LSNumber *a, *b;
		if ((a = dynamic_cast<LSNumber*>(x)) == nullptr or
			(b = dynamic_cast<LSNumber*>(y)) == nullptr) {
			LSValue::delete_temporary(x);
			LSValue::delete_temporary(y);
			jit_exception_throw(new vm::ExceptionObj(vm::Exception::NO_SUCH_OPERATOR));
		}
		auto res = (int) a->value ^ (int) b->value;
		LSValue::delete_temporary(x);
		LSValue::delete_temporary(y);
		return res;
	});
}

Compiler::value ValueSTD::op_in(Compiler& c, std::vector<Compiler::value> args) {
	if (args[1].t == Type::INTEGER) {
		return c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, int b) {
			return a->in_i(b);
		});
	} else {
		return c.insn_call(Type::BOOLEAN, args, +[](LSValue* a, LSValue* b) {
			return a->in(b);
		});
	}
}

Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args) {
	if (args[0].t == Type::BOOLEAN) {
		return c.insn_call(Type::STRING, args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	}
	if (args[0].t == Type::INTEGER) {
		return c.insn_call(Type::STRING, args, +[](int v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t == Type::LONG) {
		return c.insn_call(Type::STRING, args, +[](long v) {
			return new LSString(std::to_string(v));
		});
	}
	if (args[0].t.raw_type == RawType::MPZ) {
		auto s = c.insn_call(Type::STRING, args, +[](__mpz_struct v) {
			char buff[10000];
			mpz_get_str(buff, 10, &v);
			return new LSString(buff);
		});
		if (args[0].t.temporary) {
			c.insn_delete_mpz(args[0]);
		}
		return s;
	}
	if (args[0].t == Type::REAL) {
		return c.insn_call(Type::STRING, args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	}
	// Default type : pointer
	return c.insn_call(Type::STRING, args, (void*) &LSValue::ls_json);
}

Compiler::value ValueSTD::typeID(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_typeof(args[0]);
}

}
