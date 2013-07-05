#ifndef _NODE_MS_MAP_H
#define _NODE_MS_MAP_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSMap: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(mapObj *map);
  
  // static Handle<Value> SelectOutputFormat (const Arguments& args);
  // static Handle<Value> SetExtent (const Arguments& args);
  // static Handle<Value> DrawMap (const Arguments& args);
  // static Handle<Value> Recompute (const Arguments& args);
  // static Handle<Value> Copy (const Arguments& args);
  // static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info);
  // static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  MSMap();
  MSMap(mapObj *map);
  inline mapObj *get() { return this_; }
  mapObj *this_;

private:
  ~MSMap();
  
};
#endif
