#ifndef _NODE_MS_HASHTABLE_H
#define _NODE_MS_HASHTABLE_H

#include <nan.h>
#include <mapserver.h>
#include "utils.hpp"

class MSHashTable: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(hashTableObj *table);

  static NAN_PROPERTY_GETTER(NamedPropertyGetter);
  static NAN_PROPERTY_SETTER(NamedPropertySetter);

  MSHashTable();
  MSHashTable(hashTableObj *table);
  inline hashTableObj *get() { return this_; }
  hashTableObj *this_;

private:
  ~MSHashTable();

};
#endif
