#ifndef LSFUNCTION_H_
#define LSFUNCTION_H_

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <jit/jit.h>
#include "../LSValue.hpp"

namespace ls {

template <class T = LSValue*>
class LSFunction : public LSValue {
public:

	static LSClass* clazz;

	void* function;
	std::vector<LSValue*> captures;
	std::vector<bool> captures_native;
	// For reflexion
	std::vector<LSValue*> args;
	LSValue* return_type;

	LSFunction(void* function);
	virtual ~LSFunction();

	void add_capture(LSValue* value);
	LSValue* get_capture(int index);

	/*
	 * LSValue methods
	 */
	bool to_bool() const override;
	bool ls_not() const override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	LSValue* attr(const std::string& key) const override;
	LSValue* clone() const override;
	std::ostream& dump(std::ostream& os) const override;
	std::string json() const override;
	LSValue* getClass() const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSFunction.tcc"
#endif

#endif
