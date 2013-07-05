#include "ms_error.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSError::constructor;

void MSError::Initialize(Handle<Object> target) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSError::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("MSError"));
  
  // constructor->InstanceTemplate()->SetNamedPropertyHandler(MSError::NamedPropertyGetter, NULL, MSError::NamedPropertyQuery, NULL, MSError::NamedPropertyEnumerator);

  // target->Set(String::NewSymbol("MSError"), constructor->GetFunction());
}

MSError::MSError(errorObj *err) : ObjectWrap(), this_(err) { }

MSError::MSError() : ObjectWrap(), this_(0) { }

MSError::~MSError() { }

Handle<Value> MSError::New(const Arguments& args)
{
  HandleScope scope;

  if (!args.IsConstructCall())
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    MSError *f =  static_cast<MSError *>(ptr);
    f->Wrap(args.This());
    return args.This();
  }

  return args.This();
}

Handle<Value> MSError::New(errorObj *err) {
  HandleScope scope;
  MSError *wrapped = new MSError(err);
  Handle<Value> ext = External::New(wrapped);
  Handle<Object> obj = MSError::constructor->GetFunction()->NewInstance(1, &ext);
  return scope.Close(obj);
}

// Handle<Value> MSError::NamedPropertyGetter (Local<String> property, const AccessorInfo& info) {
//   HandleScope scope;
//   MSError *err = ObjectWrap::Unwrap<MSError>(info.This());
//   v8::String::AsciiValue n(property);
//   if (strcmp(*n, "code") == 0) {
//     return scope.Close(Integer::New(err->this_->code));
//   } else if (strcmp(*n, "codeStr") == 0) {
//     return scope.Close(String::New(msGetErrorCodeString(err->this_->code)));
//   } else if (strcmp(*n, "message") == 0) {
//     return scope.Close(String::New(err->this_->message));
//   } else if (strcmp(*n, "routine") == 0) {
//     return scope.Close(String::New(err->this_->routine));
//   }
//   return Undefined();
// }
// 
// Handle<Integer> MSError::NamedPropertyQuery(Local<String> property,
//                                    const AccessorInfo& info) {
//   HandleScope scope;
//   v8::String::AsciiValue n(property);
//   if (strcmp(*n, "code") == 0) {
//     return scope.Close(Integer::New(None));
//   } else if (strcmp(*n, "codeStr") == 0) {
//     return scope.Close(Integer::New(None));
//   } else if (strcmp(*n, "message") == 0) {
//     return scope.Close(Integer::New(None));
//   } else if (strcmp(*n, "routine") == 0) {
//     return scope.Close(Integer::New(None));
//   }
//   return Handle<Integer>();
// }
// 
// Handle<Array> MSError::NamedPropertyEnumerator(const AccessorInfo& info) {
//   HandleScope scope;
// 
//   Local<Array> env = Array::New(4);
//   env->Set(0, String::New("code"));
//   env->Set(1, String::New("codeStr"));
//   env->Set(2, String::New("message"));
//   env->Set(3, String::New("routine"));
// 
//   return scope.Close(env);
// }