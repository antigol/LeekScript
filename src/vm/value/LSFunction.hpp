#ifndef LSFUNCTION_H_
#define LSFUNCTION_H_

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include "../LSValue.hpp"
#include <map>
#include <jit/jit.h>
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSFunction : public LSValue {
public:

	static LSClass* function_class;

	void* function;
	std::vector<LSValue*> captures;

	LSFunction(void* function);
	LSFunction(Json& data);
	virtual ~LSFunction();

	void add_capture(LSValue* value);
	LSValue* get_capture(int index);

	// For reflexion
	std::vector<LSValue*> args;
	LSValue* return_type;

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSVALUE_OPERATORS

	bool eq(const LSFunction*) const override;
	bool lt(const LSFunction*) const override;


	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* clone() const;

	std::ostream& print(std::ostream& os) const;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override { return 8; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
