#ifndef _NODE_MS_OUTPUTFORMAT_H
#define _NODE_MS_OUTPUTFORMAT_H

#include <nan.h>
#include <mapserver.h>
#include "utils.hpp"

class MSOutputFormat: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(outputFormatObj *of);

  static NAN_GETTER(PropertyGetter);
  static NAN_SETTER(PropertySetter);

  MSOutputFormat();
  MSOutputFormat(outputFormatObj *of);
  inline outputFormatObj *get() { return this_; }
  outputFormatObj *this_;

private:
  ~MSOutputFormat();

};
#endif
