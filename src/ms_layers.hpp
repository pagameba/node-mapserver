#ifndef _NODE_MS_LAYERS_H
#define _NODE_MS_LAYERS_H

#include <nan.h>
#include <mapserver.h>
#include "utils.hpp"

class MSLayers: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(mapObj *map);

  static NAN_INDEX_GETTER(IndexGetter);
  static NAN_PROPERTY_GETTER(NamedGetter);

  MSLayers();
  MSLayers(mapObj *map);
  inline mapObj *get() { return this_; }
  mapObj *this_;

private:
  ~MSLayers();

};
#endif
