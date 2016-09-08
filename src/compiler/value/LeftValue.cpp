#include "LeftValue.hpp"

namespace ls {

LeftValue::LeftValue() {}

LeftValue::~LeftValue() {}

bool LeftValue::isLeftValue() const {
	return true;
}

void LeftValue::reanalyse_help(SemanticAnalyser* analyser, const Type& req_type)
{
	reanalyse_l_help(analyser, req_type, Type::UNKNOWN);
}

void LeftValue::finalize_help(SemanticAnalyser* analyser, const Type& req_type)
{
	finalize_l_help(analyser, req_type, Type::UNKNOWN);
}

}
