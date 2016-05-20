#include "ms_outputformat.hpp"

Nan::Persistent<v8::FunctionTemplate> MSOutputFormat::constructor;

void MSOutputFormat::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(MSOutputFormat::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("OutputFormat").ToLocalChecked());

  RO_ATTR(tpl, "name", PropertyGetter);
  RO_ATTR(tpl, "mimetype", PropertyGetter);
  RO_ATTR(tpl, "driver", PropertyGetter);
  RO_ATTR(tpl, "extension", PropertyGetter);
  RO_ATTR(tpl, "renderer", PropertyGetter);
  RO_ATTR(tpl, "imagemode", PropertyGetter);
  RO_ATTR(tpl, "transparent", PropertyGetter);

  target->Set(Nan::New("OutputFormat").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSOutputFormat::MSOutputFormat(outputFormatObj *of) : ObjectWrap(), this_(of) {}

MSOutputFormat::MSOutputFormat() : ObjectWrap(), this_(0) {}

MSOutputFormat::~MSOutputFormat() { }

NAN_METHOD(MSOutputFormat::New) {
  MSOutputFormat *obj;

  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void* ptr = ext->Value();
    obj = static_cast<MSOutputFormat*>(ptr);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  if (!ISSTR(info, 0) || !ISSTR(info,1)) {
    Nan::ThrowTypeError("MSOutputFormat requires two string arguments");
  }

  outputFormatObj *format = msCreateDefaultOutputFormat(NULL, TOSTR(info[0]), TOSTR(info[1]));

  /* in the case of unsupported formats, msCreateDefaultOutputFormat
     should return NULL */
  if (!format) {
    msSetError(MS_MISCERR, "Unsupported format driver: %s",
               "outputFormatObj()", TOSTR(info[0]));
    info.GetReturnValue().Set(info.This());
    return;
  }

  msInitializeRendererVTable(format);

  /* Else, continue */
  format->refcount++;
  format->inmapfile = MS_TRUE;

  obj = new MSOutputFormat(format);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSOutputFormat::NewInstance(outputFormatObj *of_ptr) {
  Nan::EscapableHandleScope scope;
  MSOutputFormat* of = new MSOutputFormat();
  of->this_ = of_ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(of);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_GETTER(MSOutputFormat::PropertyGetter) {
  MSOutputFormat *of = Nan::ObjectWrap::Unwrap<MSOutputFormat>(info.Holder());

  if (STRCMP(property, "name")) {
    info.GetReturnValue().Set(Nan::New(of->this_->name).ToLocalChecked());
  } else if (STRCMP(property, "mimetype")) {
    info.GetReturnValue().Set(Nan::New(of->this_->mimetype).ToLocalChecked());
  } else if (STRCMP(property, "driver")) {
    info.GetReturnValue().Set(Nan::New(of->this_->driver).ToLocalChecked());
  } else if (STRCMP(property, "renderer")) {
    info.GetReturnValue().Set(of->this_->renderer);
  } else if (STRCMP(property, "imagemode")) {
    info.GetReturnValue().Set(of->this_->imagemode);
  } else if (STRCMP(property, "transparent")) {
    info.GetReturnValue().Set(of->this_->transparent);
  }
}