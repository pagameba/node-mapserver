#ifndef _NODE_MS_LAYER_H
#define _NODE_MS_LAYER_H

#include <nan.h>
#include <mapserver.h>

#include "ms_hashtable.hpp"
#include "utils.hpp"

class MSLayer: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(layerObj* ptr);

  static NAN_GETTER(PropertyGetter);
  static NAN_SETTER(PropertySetter);

  static NAN_METHOD(GetGridIntersectionCoordinates);
  static NAN_METHOD(UpdateFromString);
#if MS_VERSION_NUM >= 60400
  static NAN_METHOD(ToString);
#endif

  MSLayer();
  MSLayer(layerObj *layer);
  inline layerObj *get() { return this_; }
  layerObj *this_;

private:
  ~MSLayer();

};
#endif
