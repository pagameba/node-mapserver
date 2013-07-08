#ifndef __MS_COMMON_H__
#define __MS_COMMON_H__

#include <v8.h>

#define TOSTR(obj) (*String::Utf8Value((obj)->ToString()))

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (args.Length() <= (I) || !args[I]->IsNumber())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a number")));    \
  VAR = args[I]->NumberValue();

#define REQ_FUN_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a function")));  \
  Local<Function> VAR = Local<Function>::Cast(args[I]);

#define RW_PROPERTY(TEMPL, SYM, GETTER, SETTER)                         \
  TEMPL->InstanceTemplate()->SetAccessor(String::NewSymbol(SYM), GETTER, SETTER);

#define RO_PROPERTY(TEMPL, SYM, GETTER) \
  TEMPL->InstanceTemplate()->SetAccessor(String::NewSymbol(SYM), GETTER, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

#define THROW_ERROR(TYPE, STR)                                          \
  return ThrowException(Exception::TYPE(String::New(STR)));

#define RETURN_STRING(STR)                                              \
  HandleScope scope;                                                    \
  Local<String> result = String::New(STR);                              \
  return scope.Close(result);

#define RETURN_NUMBER(NUM)                                              \
  HandleScope scope;                                                    \
  Local<Number> result = Number::New(NUM);                              \
  return scope.Close(result);
  
#define REPLACE_STRING(TARGET, VALUE)                                   \
  if (VALUE->IsString()) {                                              \
    v8::String::AsciiValue _v_(VALUE->ToString());                      \
    msFree(TARGET);                                                     \
    TARGET = strdup(*_v_);                                              \
  }


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