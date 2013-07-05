#ifndef __MS_COMMON_H__
#define __MS_COMMON_H__

#include <v8.h>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  String::Utf8Value VAR(args[I]->ToString());

#define THROW_ERROR(TYPE, STR)                                          \
  return ThrowException(Exception::TYPE(String::New(STR)));


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