#ifndef LSOBJECT_HPP_
#define LSOBJECT_HPP_

#include "../LSValue.hpp"
#include "LSClass.hpp"
#include "../../../lib/json.hpp"
#include "../Type.hpp"

namespace ls {

class LSObject : public LSValue {

public:

	std::map<std::string, LSValue*> values;
	LSClass* clazz;
	bool readonly;

	static LSValue* object_class;

	LSObject();
	LSObject(std::initializer_list<std::pair<std::string, LSValue*>>);
	LSObject(LSClass*);
	LSObject(Json& data);

	virtual ~LSObject();

	void addField(std::string name, LSValue* value);
	LSArray<LSValue*>* ls_get_keys() const;
	LSArray<LSValue*>* ls_get_values() const;

	/*
	 * LSValue methods
	 */
	bool isTrue() const override;

	LSVALUE_OPERATORS

	bool eq(const LSObject*) const override;
	bool lt(const LSObject*) const override;

	bool in(LSValue*) const override;

	LSValue* at (const LSValue* value) const override;
	LSValue** atL (const LSValue* value) override;

	LSValue* attr(const LSValue* key) const override;
	LSValue** attrL(const LSValue* key) override;

	LSValue* abso() const override;

	LSValue* clone() const override;

	std::ostream& print(std::ostream& os) const override;
	std::string json() const override;

	LSValue* getClass() const override;

	int typeID() const override { return 9; }

	virtual const BaseRawType* getRawType() const override;
};

}

#endif
