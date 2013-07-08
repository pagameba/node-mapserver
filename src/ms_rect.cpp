#include "ms_rect.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSRect::constructor;

void MSRect::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSRect::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Rect"));
  
  RW_PROPERTY(constructor, "minx", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "miny", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "maxx", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "maxy", PropertyGetter, PropertySetter);
  
  target->Set(String::NewSymbol("Rect"), constructor->GetFunction());
}

MSRect::MSRect(rectObj *rect) : ObjectWrap(), this_(rect) {}

MSRect::MSRect() : ObjectWrap(), this_(0) {}

MSRect::~MSRect() { }

Handle<Value> MSRect::New(const Arguments &args) {
  HandleScope scope;
  MSRect *obj;
  
  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSRect*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }
  
  rectObj *rect = (rectObj *)calloc(1, sizeof(rectObj));
  if(!rect) {
    return args.This();
  }

  rect->minx = -1;
  rect->miny = -1;
  rect->maxx = -1;
  rect->maxy = -1;
  
  obj = new MSRect(rect);
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> MSRect::New(rectObj *rect) {
  return ClosedPtr<MSRect, rectObj>::Closed(rect);
}

Handle<Value> MSRect::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSRect *rect = ObjectWrap::Unwrap<MSRect>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "minx") == 0) {
    RETURN_NUMBER(rect->this_->minx);
  } else if (strcmp(*n, "miny") == 0) {
    RETURN_NUMBER(rect->this_->miny);
  } else if (strcmp(*n, "maxx") == 0) {
    RETURN_NUMBER(rect->this_->maxx);
  } else if (strcmp(*n, "maxy") == 0) {
    RETURN_NUMBER(rect->this_->maxy);
  }
  return Undefined();
}

void MSRect::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSRect *rect = ObjectWrap::Unwrap<MSRect>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "minx") == 0) {
    rect->this_->minx = value->NumberValue();
  } else if (strcmp(*n, "miny") == 0) {
    rect->this_->miny = value->NumberValue();
  } else if (strcmp(*n, "maxx") == 0) {
    rect->this_->maxx = value->NumberValue();
  } else if (strcmp(*n, "maxy") == 0) {
    rect->this_->maxy = value->NumberValue();
  }
}