#ifndef _NODE_MS_PROJECTION_H
#define _NODE_MS_PROJECTION_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSProjection: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(projectionObj *proj);
  
  static Handle<Value> ProjectPoint (const Arguments& args);
  static Handle<Value> GetMapUnits (Local<String> property, const AccessorInfo& info);
  
  // static Handle<Value> SetExtent (const Arguments& args);
  // static Handle<Value> DrawMap (const Arguments& args);
  // static Handle<Value> Recompute (const Arguments& args);
  // static Handle<Value> Copy (const Arguments& args);
  // static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info);
  // static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  MSProjection();
  MSProjection(projectionObj *proj);
  inline projectionObj *get() { return this_; }
  projectionObj *this_;

private:
  ~MSProjection();
  
};
#endif
