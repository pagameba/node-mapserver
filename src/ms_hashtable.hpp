#ifndef _NODE_MS_HASHTABLE_H
#define _NODE_MS_HASHTABLE_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSHashTable: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(hashTableObj *table);
  
  static Handle<Value> NamedGetter (Local<String> property, const AccessorInfo& info);
  
  MSHashTable();
  MSHashTable(hashTableObj *table);
  inline hashTableObj *get() { return this_; }
  hashTableObj *this_;

private:
  ~MSHashTable();
  
};
#endif
