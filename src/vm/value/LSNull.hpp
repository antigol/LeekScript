#ifndef LSNULL
#define LSNULL

#include <iostream>
#include <string>
#include "../LSValue.hpp"
#include "../Type.hpp"

namespace ls {

class LSNull : public LSValue {
private:
	static LSValue* null_var;
	LSNull();

public:

	static LSValue* get();
	static LSClass* null_class;

	~LSNull();

	LSValue* clone() const override;

	bool isTrue() const override;

	LSVALUE_OPERATORS

	bool eq(const LSNull*) const override;
	bool lt(const LSNull*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override { return 1; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
