#ifndef __NODE_MAPSERVER_UTILS_H__
#define __NODE_MAPSERVER_UTILS_H__

#define REPLACE_STRING(TARGET, VALUE)                                   \
  if (VALUE->IsString()) {                                              \
    msFree(TARGET);                                                     \
    TARGET = strdup(*v8::String::Utf8Value((VALUE)->ToString()));                    \
  }

#define STRCMP(A, B) \
  !std::strcmp(*v8::String::Utf8Value(A), B)

#define ISSTR(INFO, N) \
  (INFO.Length() > N && INFO[N]->IsString())

#define ISNUMERIC(INFO, N) \
  (INFO.Length() > N && INFO[N]->IsNumber())

#define TOSTR(VALUE) (*v8::String::Utf8Value((VALUE)->ToString()))

#define RO_ATTR(tpl, name, get) \
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New<v8::String>(name).ToLocalChecked(), get);

#define RW_ATTR(tpl, name, get, set) \
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New<v8::String>(name).ToLocalChecked(), get, set);

#define NODE_MAPSERVER_DEFINE_CONSTANT(t, n, v) \
  (t)->Set(Nan::New<v8::String>(n).ToLocalChecked(), Nan::New<v8::Number>(v));

#endif