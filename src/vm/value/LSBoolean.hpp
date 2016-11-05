#ifndef LSBOOLEAN_HPP_
#define LSBOOLEAN_HPP_

#include <string>
#include "../../../lib/json.hpp"
#include "../LSValue.hpp"
#include "../Type.hpp"

namespace ls {

class LSBoolean : public LSValue {
private:

	LSBoolean();
	LSBoolean(bool value);

public:

	const bool value;

	static LSValue* boolean_class;
	static LSBoolean* false_val;
	static LSBoolean* true_val;
	static LSBoolean* get(bool value) {
		return value ? true_val : false_val;
	}

	LSBoolean(Json& data);

	virtual ~LSBoolean();

	bool isTrue() const override;

	LSValue* ls_not() override;
	LSValue* ls_tilde() override;

	LSVALUE_OPERATORS

	LSValue* ls_add(LSNumber*) override;
	LSValue* ls_add(LSString* s) override;
	LSValue* ls_sub(LSNumber*) override;

	bool eq(const LSBoolean*) const override;
	bool lt(const LSBoolean*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* clone() const;

	std::ostream& print(std::ostream& os) const;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override { return 2; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
