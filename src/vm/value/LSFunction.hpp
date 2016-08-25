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
	std::map<std::string, LSValue*> values;

	LSFunction(void* function);
	LSFunction(void* function, int refs, bool native);
	LSFunction(Json& data);

	virtual ~LSFunction();

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
