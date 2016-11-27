#include <iostream>
#include "LSValue.hpp"
#include "value/LSNumber.hpp"
#include "value/LSNull.hpp"
#include "value/LSBoolean.hpp"
#include "value/LSArray.hpp"
#include "value/LSMap.hpp"
#include "value/LSSet.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "VM.hpp"

using namespace std;

namespace ls {

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;

LSValue::LSValue() : refs(0) {
	obj_count++;
	#if DEBUG_LEAKS_DETAILS
		objs().insert({this, this});
	#endif
}

LSValue::LSValue(const LSValue& ) : refs(0) {
	obj_count++;
	#if DEBUG_LEAKS_DETAILS
		objs().insert({this, this});
	#endif
}

LSValue::~LSValue() {
	obj_deleted++;
	#if DEBUG_LEAKS_DETAILS
		objs().erase(this);
	#endif
}

LSValue* LSValue::ls_minus()                                 { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_not()                                   { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_tilde()                                 { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_preinc()                                { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_inc()                                   { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_predec()                                { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_dec()                                   { delete_temporary(this); return LSNull::get(); }

LSValue* LSValue::ls_add(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_add(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_add(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_add(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_add_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_add_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_add_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_add_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_sub(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_sub(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_sub_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_sub_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_sub_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_sub_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_mul(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mul(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_mul_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_mul_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_mul_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mul_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_div(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_div(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_div(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_div(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_int_div(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_int_div(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_div_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_div_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_div_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_div_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_pow(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_pow(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_pow_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_pow_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_pow_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_pow_eq(LSClass* value)                  { delete_temporary(value); return this; }

LSValue* LSValue::ls_mod(LSNull*)                            { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSBoolean*)                         { delete_temporary(this); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSNumber* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSString* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<LSValue*>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<int>* value)                { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSArray<double>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,LSValue*>* value)    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,int>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<LSValue*,double>* value)      { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,LSValue*>* value)         { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,int>* value)              { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSMap<int,double>* value)           { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<LSValue*>* value)             { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<int>* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSSet<double>* value)               { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSObject* value)                    { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSFunction* value)                  { delete_temporary(this); delete_temporary(value); return LSNull::get(); }
LSValue* LSValue::ls_mod(LSClass* value)                     { delete_temporary(this); delete_temporary(value); return LSNull::get(); }

LSValue* LSValue::ls_mod_eq(LSNull*)                         { return this; }
LSValue* LSValue::ls_mod_eq(LSBoolean*)                      { return this; }
LSValue* LSValue::ls_mod_eq(LSNumber* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSString* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<LSValue*>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<int>* value)             { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSArray<double>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,LSValue*>* value) { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,int>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<LSValue*,double>* value)   { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,LSValue*>* value)      { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,int>* value)           { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSMap<int,double>* value)        { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<LSValue*>* value)          { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<int>* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSSet<double>* value)            { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSObject* value)                 { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSFunction* value)               { delete_temporary(value); return this; }
LSValue* LSValue::ls_mod_eq(LSClass* value)                  { delete_temporary(value); return this; }

bool LSValue::eq(const LSNull*) const                        { return false; }
bool LSValue::eq(const LSBoolean*) const                     { return false; }
bool LSValue::eq(const LSNumber*) const                      { return false; }
bool LSValue::eq(const LSString*) const                      { return false; }
bool LSValue::eq(const LSArray<LSValue*>*) const             { return false; }
bool LSValue::eq(const LSArray<int>*) const                  { return false; }
bool LSValue::eq(const LSArray<double>*) const               { return false; }
bool LSValue::eq(const LSMap<LSValue*,LSValue*>*) const      { return false; }
bool LSValue::eq(const LSMap<LSValue*,int>*) const           { return false; }
bool LSValue::eq(const LSMap<LSValue*,double>*) const        { return false; }
bool LSValue::eq(const LSMap<int,LSValue*>*) const           { return false; }
bool LSValue::eq(const LSMap<int,int>*) const                { return false; }
bool LSValue::eq(const LSMap<int,double>*) const             { return false; }
bool LSValue::eq(const LSMap<double,LSValue*>*) const        { return false; }
bool LSValue::eq(const LSMap<double,int>*) const             { return false; }
bool LSValue::eq(const LSMap<double,double>*) const          { return false; }
bool LSValue::eq(const LSSet<LSValue*>*) const               { return false; }
bool LSValue::eq(const LSSet<int>*) const                    { return false; }
bool LSValue::eq(const LSSet<double>*) const                 { return false; }
bool LSValue::eq(const LSFunction*) const                    { return false; }
bool LSValue::eq(const LSObject*) const                      { return false; }
bool LSValue::eq(const LSClass*) const                       { return false; }

bool LSValue::lt(const LSNull*) const                        { return typeID() < 1; }
bool LSValue::lt(const LSBoolean*) const                     { return typeID() < 2; }
bool LSValue::lt(const LSNumber*) const                      { return typeID() < 3; }
bool LSValue::lt(const LSString*) const                      { return typeID() < 4; }
bool LSValue::lt(const LSArray<LSValue*>*) const             { return typeID() < 5; }
bool LSValue::lt(const LSArray<int>*) const                  { return typeID() < 5; }
bool LSValue::lt(const LSArray<double>*) const               { return typeID() < 5; }
bool LSValue::lt(const LSMap<LSValue*,LSValue*>*) const      { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,int>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<LSValue*,double>*) const        { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,LSValue*>*) const           { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,int>*) const                { return typeID() < 6; }
bool LSValue::lt(const LSMap<int,double>*) const             { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,LSValue*>*) const        { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,int>*) const             { return typeID() < 6; }
bool LSValue::lt(const LSMap<double,double>*) const          { return typeID() < 6; }
bool LSValue::lt(const LSSet<LSValue*>*) const               { return typeID() < 7; }
bool LSValue::lt(const LSSet<int>*) const                    { return typeID() < 7; }
bool LSValue::lt(const LSSet<double>*) const                 { return typeID() < 7; }
bool LSValue::lt(const LSFunction*) const                    { return typeID() < 8; }
bool LSValue::lt(const LSObject*) const                      { return typeID() < 9; }
bool LSValue::lt(const LSClass*) const                       { return typeID() < 10; }

LSValue* LSValue::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSValue::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSValue::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}

LSValue** LSValue::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSValue::range(int, int) const {
	return clone();
}

LSValue* LSValue::rangeL(int, int) {
	return this;
}

LSValue* LSValue::abso() const {
	return LSNull::get();
}

//std::ostream& operator << (std::ostream& os, LSValue& value) {
//	value.print(os);
//	return os;
//}

bool LSValue::isInteger() const {
	if (const LSNumber* v = dynamic_cast<const LSNumber*>(this)) {
		return v->isInteger();
	}
	return false;
}

LSValue* get_value(int type, Json& json) {
	switch (type) {
		case 1: return LSNull::get();
		case 2: return new LSBoolean(json);
		case 3: return new LSNumber(json);
		case 4: return new LSString(json);
		case 5: return new LSArray<LSValue*>(json);
//		case 6: return new LSMap<LSValue*,LSValue*>(json); TODO
//		case 7: return new LSSet<LSValue*>(json);
		case 8: return new LSFunction(json);
		case 9: return new LSObject(json);
		case 10: return new LSClass(json);
	}
	return LSNull::get();
}

LSValue* LSValue::parse(Json& json) {

	int type = json["t"];
	Json data = json["v"];
	return get_value(type, data);
}

std::string LSValue::to_json() const {
	return "{\"t\":" + to_string(typeID()) + ",\"v\":" + json() + "}";
}

LSString* LSValue::ls_json() {
	LSString* json = new LSString(this->json());
	LSValue::delete_temporary(this);
	return json;
}

}

namespace std {
	std::string to_string(ls::LSValue* value) {
		std::ostringstream oss;
		value->print(oss);
		return oss.str();
	}
	std::ostream& operator << (std::ostream& os, const ls::LSValue* value) {
		value->print(os);
		return os;
	}
}
