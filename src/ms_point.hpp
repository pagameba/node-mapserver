#ifndef _NODE_MS_POINT_H
#define _NODE_MS_POINT_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>
#include "ms_projection.hpp"

using namespace v8;
using namespace node;

class MSPoint: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(pointObj *point);
  
  static Handle<Value> PropertyGetter (Local<String> property, const AccessorInfo& info);
  static void PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  static Handle<Value> Project(const Arguments &args);
  static Handle<Value> DistanceToPoint(const Arguments &args);
  
  MSPoint();
  MSPoint(pointObj *point);
  inline pointObj *get() { return this_; }
  pointObj *this_;

private:
  ~MSPoint();
  
};
#endif
