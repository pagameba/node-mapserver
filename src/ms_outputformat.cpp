#include "ms_outputformat.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSOutputFormat::constructor;

void MSOutputFormat::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSOutputFormat::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Rect"));
  
  RO_PROPERTY(constructor, "name", PropertyGetter);
  RO_PROPERTY(constructor, "mimetype", PropertyGetter);
  RO_PROPERTY(constructor, "driver", PropertyGetter);
  RO_PROPERTY(constructor, "extension", PropertyGetter);
  RO_PROPERTY(constructor, "renderer", PropertyGetter);
  RO_PROPERTY(constructor, "imagemode", PropertyGetter);
  RO_PROPERTY(constructor, "transparent", PropertyGetter);
  // expose format options perhaps?
  // RO_PROPERTY(constructor, "options", PropertyGetter);
  
  target->Set(String::NewSymbol("OutputFomrat"), constructor->GetFunction());
}

MSOutputFormat::MSOutputFormat(outputFormatObj *of) : ObjectWrap(), this_(of) {}

MSOutputFormat::MSOutputFormat() : ObjectWrap(), this_(0) {}

MSOutputFormat::~MSOutputFormat() { }

Handle<Value> MSOutputFormat::New(const Arguments &args) {
  HandleScope scope;
  MSOutputFormat *obj;
  
  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSOutputFormat*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }
  
  REQ_STR_ARG(0, driver);
  REQ_STR_ARG(1, name);
  
  outputFormatObj *format = msCreateDefaultOutputFormat(NULL, *driver, *name);

  /* in the case of unsupported formats, msCreateDefaultOutputFormat
     should return NULL */
  if (!format) {
    msSetError(MS_MISCERR, "Unsupported format driver: %s",
               "outputFormatObj()", *driver);
    return args.This();
  }

  msInitializeRendererVTable(format);

  /* Else, continue */
  format->refcount++;
  format->inmapfile = MS_TRUE;
  
  obj = new MSOutputFormat(format);
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> MSOutputFormat::New(outputFormatObj *of) {
  return ClosedPtr<MSOutputFormat, outputFormatObj>::Closed(of);
}

Handle<Value> MSOutputFormat::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSOutputFormat *of = ObjectWrap::Unwrap<MSOutputFormat>(info.This());
  v8::String::AsciiValue n(property);
  
  if (strcmp(*n, "name") == 0) {
    RETURN_STRING(of->this_->name);
  } else if (strcmp(*n, "mimetype") == 0) {
    RETURN_STRING(of->this_->mimetype);
  } else if (strcmp(*n, "driver") == 0) {
    RETURN_STRING(of->this_->driver);
  } else if (strcmp(*n, "renderer") == 0) {
    RETURN_NUMBER(of->this_->renderer);
  } else if (strcmp(*n, "imagemode") == 0) {
    RETURN_NUMBER(of->this_->imagemode);
  } else if (strcmp(*n, "transparent") == 0) {
    RETURN_NUMBER(of->this_->transparent);
  }
  return Undefined();
}