#ifndef __NODE_MS_ERROR_H__
#define __NODE_MS_ERROR_H__

#include <nan.h>
#include <mapserver.h>
#include "utils.hpp"

class MSError: public Nan::ObjectWrap {
  public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;

    static void Initialize(v8::Local<v8::Object> target);
    static NAN_METHOD(New);
    static v8::Local<v8::Value> NewInstance(errorObj* err_ptr);

    MSError();
    MSError(errorObj *err);
    inline errorObj *get() { return this_; }

  protected:
    static NAN_PROPERTY_GETTER(NamedPropertyGetter);
    static NAN_PROPERTY_QUERY(NamedPropertyQuery);
    static NAN_PROPERTY_ENUMERATOR(NamedPropertyEnumerator);

  private:
    ~MSError();
    errorObj *this_;
};

#endif
