#include "../../compiler/value/Reference.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../value/ArrayAccess.hpp"
#include "../value/VariableValue.hpp"

using namespace std;

namespace ls {

Reference::Reference(Value* value)
	: Value(), value(value), var(nullptr)
{
	type = Type::VALUE;
	scope = VarScope::LOCAL;
}

Reference::~Reference() {
	delete value;
}

void Reference::print(ostream& os, int, bool debug) const {
	os << "@" << value->to_string();
	if (debug) {
		os << " " << type;
	}
}

Location Reference::location() const {
	return value->location(); // TODO take the '@'
}

void Reference::will_be_in_array(SemanticAnalyser*) {
	in_array = true;
}

void Reference::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	//std::cout << "Reference analyse() in_array " << in_array << std::endl;

	if (auto variable = dynamic_cast<VariableValue*>(value)) {

		var = analyser->get_var(variable->token.get());

		if (var != nullptr) {
			if (in_array) {
				var->must_be_pointer(analyser);
			}
			type = var->type;
			name = var->name;
			scope = var->scope;
			attr_types = var->attr_types;
			if (scope != VarScope::INTERNAL and var->function != analyser->current_function()) {
				capture_index = analyser->current_function()->capture(var);
				scope = VarScope::CAPTURE;
	  		}
		}
	} else if (auto array_access = dynamic_cast<ArrayAccess*>(value)) {

		array_access->analyse(analyser, Type::POINTER);
		auto array = array_access->array;
		type = array->type.getElementType();
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	if (req_type.raw_type == RawType::REAL) {
		type.raw_type = RawType::REAL;
	}
	type.reference = true;
//	cout << "ref " << variable->content << " : " << type << endl;
}

Compiler::value Reference::compile(Compiler& c) const {

	if (scope == VarScope::CAPTURE) {
		return c.insn_get_capture(capture_index, type);
	}

	if (name != "") {
		jit_value_t v;
		if (scope == VarScope::INTERNAL) {
			v = c.vm->internals.at(name);
		} else if (scope == VarScope::LOCAL) {
			v = c.get_var(name).v;
		} else { /* if (scope == VarScope::PARAMETER) */
			v = jit_value_get_param(c.F, 1 + var->index); // 1 offset for function ptr
		}
		if (var->type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, v, var->type), type};
		}
		if (var->type.raw_type == RawType::INTEGER and type.raw_type == RawType::REAL) {
			return {VM::int_to_real(c.F, v), type};
		}
		return {v, type};

	} else {

		auto v = ((LeftValue*) value)->compile_l(c);
		auto vv = c.insn_load(v, 0, type);
		return vv;
	}
}

Value* Reference::clone() const {
	auto r = new Reference(value->clone());
	return r;
}

}
