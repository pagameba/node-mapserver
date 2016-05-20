#ifndef _NODE_MS_RECT_H
#define _NODE_MS_RECT_H

#include <nan.h>
#include <mapserver.h>
#include "ms_projection.hpp"

class MSRect: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(rectObj *rect);

  static NAN_GETTER(PropertyGetter);
  static NAN_SETTER(PropertySetter);

  static NAN_METHOD(Project);

  MSRect();
  MSRect(rectObj *rect);
  inline rectObj *get() { return this_; };
  rectObj *this_;
  bool owner;

private:
  ~MSRect();

};
#endif
