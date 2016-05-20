#ifndef _NODE_MS_PROJECTION_H
#define _NODE_MS_PROJECTION_H

#include <nan.h>
#include <mapserver.h>
#include "utils.hpp"

class MSProjection: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(projectionObj *proj);

  static NAN_GETTER(Units);
  static NAN_GETTER(ProjString);
  static NAN_SETTER(SetProjString);

  MSProjection();
  MSProjection(projectionObj *proj);
  inline projectionObj *get() { return this_; }
  projectionObj *this_;
  bool owner;

private:
  ~MSProjection();

};
#endif
