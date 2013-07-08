#ifndef _NODE_MS_LAYER_H
#define _NODE_MS_LAYER_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSLayer: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(layerObj *layer);
  
  static Handle<Value> PropertyGetter (Local<String> property, const AccessorInfo& info);
  static void PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
  MSLayer();
  MSLayer(layerObj *layer);
  inline layerObj *get() { return this_; }
  layerObj *this_;

private:
  ~MSLayer();
  
};
#endif
