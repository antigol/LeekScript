#include "ValueSTD.hpp"
#include "../value/LSBoolean.hpp"
#include "../value/LSString.hpp"
#include "../value/LSNumber.hpp"

namespace ls {

ValueSTD::ValueSTD() : Module("Value") {

	static_field("unknown", Type::UNKNOWN, (void*) &ValueSTD::unknown);

	/*
	 * Operators
	 */
	operator_("instanceof", {
		{Type::UNKNOWN, Type::CLASS, Type::BOOLEAN, (void*) &ValueSTD::op_instanceof, Method::NATIVE}
	});
	operator_("<", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_lt, Method::NATIVE}
	});
	operator_(">", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_gt, Method::NATIVE}
	});
	operator_("and", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_and, Method::NATIVE}
	});
	operator_("or", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_or, Method::NATIVE}
	});
	operator_("xor", {
		{Type::UNKNOWN, Type::UNKNOWN, Type::BOOLEAN, (void*) &ValueSTD::op_xor, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("string", {
		{Type::UNKNOWN, Type::STRING, {}, (void*) &ValueSTD::to_string, Method::NATIVE}
	});
	method("typeID", {
		{Type::UNKNOWN, Type::INTEGER, {}, (void*) &ValueSTD::typeID, Method::NATIVE}
	});
}

jit_value_t ValueSTD::unknown(jit_function_t F) {
	return LS_CREATE_POINTER(F,
		LSNumber::get(floor(1 + ((double) rand() / RAND_MAX) * 100))
	);
}

Compiler::value ValueSTD::op_instanceof(Compiler& c, std::vector<Compiler::value> args) {
	auto r = c.insn_eq(c.insn_class_of(args[0]), args[1]);
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return r;
}

Compiler::value ValueSTD::op_lt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_lt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return res;
}

Compiler::value ValueSTD::op_gt(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_gt(c.insn_typeof(args[0]), c.insn_typeof(args[1]));
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return res;
}

Compiler::value ValueSTD::op_and(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_and(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return res;
}

Compiler::value ValueSTD::op_or(Compiler& c, std::vector<Compiler::value> args) {
	auto res = c.insn_or(c.insn_to_bool(args[0]), c.insn_to_bool(args[1]));
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
	return res;
}

Compiler::value ValueSTD::op_xor(Compiler& c, std::vector<Compiler::value> args) {
	auto a = c.insn_to_not_bool(args[0]);
	auto b = c.insn_to_not_bool(args[1]);
	auto r = c.insn_or(
		c.insn_and(a, c.insn_not(b)),
		c.insn_and(b, c.insn_not(a))
	);
	c.insn_delete(args[0]);
	c.insn_delete(args[1]);
 	return r;
 }


Compiler::value ValueSTD::to_string(Compiler& c, std::vector<Compiler::value> args) {
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
	if (args[0].t == Type::REAL) {
		return c.insn_call(Type::STRING, args, +[](double v) {
			return new LSString(LSNumber::print(v));
		});
	}
	if (args[0].t == Type::BOOLEAN) {
		return c.insn_call(Type::STRING, args, +[](bool b) {
			return new LSString(b ? "true" : "false");
		});
	}
	if (args[0].t.nature == Nature::POINTER) {
		return c.insn_call(Type::STRING, args, (void*) &LSValue::ls_json);
	}
	std::cout << "Type non supporté !" << std::endl;
	throw new std::exception();
	return {nullptr, Type::VOID};
}

Compiler::value ValueSTD::typeID(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_typeof(args[0]);
}

}
