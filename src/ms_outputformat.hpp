#ifndef _NODE_MS_OUTPUTFORMAT_H
#define _NODE_MS_OUTPUTFORMAT_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSOutputFormat: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(outputFormatObj *of);
  
  static Handle<Value> PropertyGetter (Local<String> property, const AccessorInfo& info);
  static void PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  MSOutputFormat();
  MSOutputFormat(outputFormatObj *of);
  inline outputFormatObj *get() { return this_; }
  outputFormatObj *this_;

private:
  ~MSOutputFormat();
  
};
#endif
