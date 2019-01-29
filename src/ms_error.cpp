#include "ms_error.hpp"

Nan::Persistent<v8::FunctionTemplate> MSError::constructor;

void MSError::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New <v8::FunctionTemplate>(MSError::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("MSError").ToLocalChecked());
  Nan::SetPrototypeMethod(tpl, "toString", ToString);
  Nan::SetPrototypeMethod(tpl, "toDetailString", ToString);

  Nan::SetNamedPropertyHandler(
        tpl->InstanceTemplate()
      , MSError::NamedPropertyGetter
      , NULL
      , MSError::NamedPropertyQuery
      , NULL
      , MSError::NamedPropertyEnumerator);

  target->Set(Nan::New("MSError").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSError::MSError(errorObj *err) : Nan::ObjectWrap(), this_(err) { }

MSError::MSError() : Nan::ObjectWrap(), this_(0) { }

MSError::~MSError() { }

NAN_METHOD(MSError::New)
{
  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
    return;
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void *ptr = ext->Value();
    MSError *f =  static_cast<MSError *>(ptr);
    f->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSError::NewInstance(errorObj *err_ptr) {
  Nan::EscapableHandleScope scope;
  MSError* err = new MSError();
  err->this_ = err_ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(err);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_METHOD(MSError::ToString) {
  MSError *err = Nan::ObjectWrap::Unwrap<MSError>(info.Holder());
  info.GetReturnValue().Set(Nan::New(err->this_->message).ToLocalChecked());
}

NAN_PROPERTY_GETTER(MSError::NamedPropertyGetter) {
  MSError *err = Nan::ObjectWrap::Unwrap<MSError>(info.Holder());

  if (STRCMP(property, "code")) {
    info.GetReturnValue().Set(err->this_->code);
  } else if (STRCMP(property, "message")) {
    info.GetReturnValue().Set(Nan::New(err->this_->message).ToLocalChecked());
  } else if (STRCMP(property, "routine")) {
    info.GetReturnValue().Set(Nan::New(err->this_->routine).ToLocalChecked());
  }
}

NAN_PROPERTY_QUERY(MSError::NamedPropertyQuery) {
  if (STRCMP(property, "code")) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(v8::None));
  } else if (STRCMP(property, "message")) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(v8::None));
  } else if (STRCMP(property, "routine")) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(v8::None));
  } else {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(v8::DontEnum));
  }
}

NAN_PROPERTY_ENUMERATOR(MSError::NamedPropertyEnumerator) {
  v8::Local<v8::Array> env = Nan::New<v8::Array>(4);
  Nan::Set(env, 0, Nan::New("code").ToLocalChecked());
  Nan::Set(env, 2, Nan::New("message").ToLocalChecked());
  Nan::Set(env, 3, Nan::New("routine").ToLocalChecked());
  info.GetReturnValue().Set(env);
}