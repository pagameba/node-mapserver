#ifndef __MS_COMMON_H__
#define __MS_COMMON_H__

#include <v8.h>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

template <typename T, typename K>
class ClosedPtr {
public:
  static v8::Handle<v8::Value> Closed(K *raw) {
    v8::HandleScope scope;
    T *wrapped = new T(raw);
    v8::Handle<v8::Value> ext = v8::External::New(wrapped);
    v8::Handle<v8::Object> obj = T::constructor->GetFunction()->NewInstance(1, &ext);
    return scope.Close(obj);
  }
};

#endif