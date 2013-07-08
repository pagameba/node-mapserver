#ifndef _NODE_MS_LAYERS_H
#define _NODE_MS_LAYERS_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>
#include "ms_layer.hpp"

using namespace v8;
using namespace node;

class MSLayers: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(mapObj *map);
  
  static Handle<Value> IndexGetter(uint32_t index, const AccessorInfo& info);
  static Handle<Value> NamedGetter (Local<String> property, const AccessorInfo& info);
  
  MSLayers();
  MSLayers(mapObj *map);
  inline mapObj *get() { return this_; }
  mapObj *this_;

private:
  ~MSLayers();
  
};
#endif
