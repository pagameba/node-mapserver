#ifndef _NODE_MS_RECT_H
#define _NODE_MS_RECT_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>
#include "ms_projection.hpp"

using namespace v8;
using namespace node;

class MSRect: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(rectObj *rect);
  static Handle<Value> Project(const Arguments &args);

  static Handle<Value> PropertyGetter (Local<String> property, const AccessorInfo& info);
  static void PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);

  MSRect();
  MSRect(rectObj *rect);
  inline rectObj *get() { return this_; };
  rectObj *this_;
  bool owner;

private:
  ~MSRect();

};
#endif
