#include <iostream>
#include "LSValue.hpp"
#include "value/LSVar.hpp"
#include "value/LSVec.hpp"
#include "value/LSMap.hpp"
#include "value/LSSet.hpp"
#include "VM.hpp"

using namespace std;

namespace ls {

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;
#if DEBUG >= 4
	extern std::map<LSValue*, LSValue*> objs;
#endif

LSValue::LSValue() : refs(0) {
	obj_count++;
	#if DEBUG >= 4
		objs.insert({this, this});
	#endif
}

LSValue::LSValue(const LSValue& ) : refs(0) {
	obj_count++;
	#if DEBUG >= 4
		objs.insert({this, this});
	#endif
}

LSValue::~LSValue() {
	obj_deleted++;
	#if DEBUG >= 4
		objs.erase(this);
	#endif
}

bool LSValue::eq(const LSVar*) const                         { return false; }
bool LSValue::eq(const LSVec<LSValue*>*) const               { return false; }
bool LSValue::eq(const LSVec<void*>*) const                  { return false; }
bool LSValue::eq(const LSVec<int32_t>*) const                { return false; }
bool LSValue::eq(const LSVec<int64_t>*) const                { return false; }
bool LSValue::eq(const LSVec<float>*) const                  { return false; }
bool LSValue::eq(const LSVec<double>*) const                 { return false; }
bool LSValue::eq(const LSMap<LSValue*,LSValue*>*) const      { return false; }
bool LSValue::eq(const LSMap<LSValue*,int>*) const           { return false; }
bool LSValue::eq(const LSMap<LSValue*,double>*) const        { return false; }
bool LSValue::eq(const LSMap<int,LSValue*>*) const           { return false; }
bool LSValue::eq(const LSMap<int,int>*) const                { return false; }
bool LSValue::eq(const LSMap<int,double>*) const             { return false; }
bool LSValue::eq(const LSSet<LSValue*>*) const               { return false; }
bool LSValue::eq(const LSSet<void*>*) const                  { return false; }
bool LSValue::eq(const LSSet<int32_t>*) const                { return false; }
bool LSValue::eq(const LSSet<int64_t>*) const                { return false; }
bool LSValue::eq(const LSSet<float>*) const                  { return false; }
bool LSValue::eq(const LSSet<double>*) const                 { return false; }

bool LSValue::lt(const LSVar*) const                         { return typeID() < 4; }
bool LSValue::lt(const LSVec<void*>*) const                  { return typeID() < 5; }
bool LSValue::lt(const LSVec<LSValue*>*) const               { return typeID() < 5; }
bool LSValue::lt(const LSVec<int32_t>*) const                { return typeID() < 5; }
bool LSValue::lt(const LSVec<int64_t>*) const                { return typeID() < 5; }
bool LSValue::lt(const LSVec<float>*) const                  { return typeID() < 5; }
bool LSValue::lt(const LSVec<double>*) const                 { return typeID() < 5; }
bool LSValue::lt(const LSMap<LSValue*,LSValue*>*) const      { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,int>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,double>*) const        { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,LSValue*>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,int>*) const                { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,double>*) const             { return typeID() < 6; }
bool LSValue::lt(const LSSet<LSValue*>*) const               { return typeID() < 7; }
bool LSValue::lt(const LSSet<int>*) const                    { return typeID() < 7; }
bool LSValue::lt(const LSSet<double>*) const                 { return typeID() < 7; }

LSValue* get_value(int type, Json& json) {
	switch (type) {
//		case 1: return new LSVar();
//		case 2: return new LSBoolean(json);
//		case 3: return new LSNumber(json);
//		case 4: return new LSString(json);
//		case 5: return new LSVec<LSValue*>(json);
//		case 6: return new LSMap<LSValue*,LSValue*>(json); TODO
//		case 7: return new LSSet<LSValue*>(json);
//		case 8: return new LSFunction(json);
//		case 9: return new LSObject(json);
//		case 10: return new LSClass(json);
	}
	return new LSVar();
}

LSValue* LSValue::parse(Json& json) {

	int type = json["t"];
	Json data = json["v"];
	return get_value(type, data);
}

std::string LSValue::to_json() const {
	return "{\"t\":" + to_string(typeID()) + ",\"v\":" + json() + "}";
}

}

