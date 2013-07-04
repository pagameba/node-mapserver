#ifndef _NODE_MAPSERVER_MAP_H
#define _NODE_MAPSERVER_MAP_H

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <string.h>
#include <assert.h>

using namespace v8;
using namespace node;

class Map: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  
  static Handle<Value> SelectOutputFormat (const Arguments& args);
  static Handle<Value> SetExtent (const Arguments& args);
  static Handle<Value> DrawMap (const Arguments& args);
  static Handle<Value> Recompute (const Arguments& args);
  static Handle<Value> Copy (const Arguments& args);
  static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info);
  static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  void _ref() { Ref(); }
  void _unref() { Unref(); }
  
  inline map_ptr get() { return _map; }
private:
  mapObj * _map;
  ~Map();
  
  
};
#endif
