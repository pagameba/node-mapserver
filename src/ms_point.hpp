#ifndef _NODE_MS_POINT_H
#define _NODE_MS_POINT_H

#include <nan.h>
#include <mapserver.h>
#include "ms_projection.hpp"
#include "utils.hpp"

class MSPoint: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(pointObj *point);

  static NAN_GETTER(PropertyGetter);
  static NAN_SETTER(PropertySetter);

  static NAN_METHOD(Project);
  static NAN_METHOD(DistanceToPoint);

  MSPoint();
  MSPoint(pointObj *point);
  inline pointObj *get() { return this_; }
  bool owner;

private:
  ~MSPoint();
  pointObj *this_;

};
#endif
