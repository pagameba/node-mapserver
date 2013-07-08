#include "ms_layer.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSLayer::constructor;

void MSLayer::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSLayer::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Layer"));
  
  RW_PROPERTY(constructor, "name", PropertyGetter, PropertySetter);
  
  target->Set(String::NewSymbol("Layer"), constructor->GetFunction());
}

MSLayer::MSLayer(layerObj *layer) : ObjectWrap(), this_(layer) {}

MSLayer::MSLayer() : ObjectWrap(), this_(0) {}

MSLayer::~MSLayer() { }

Handle<Value> MSLayer::New(const Arguments &args) {
  HandleScope scope;
  MSLayer *obj;
  
  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSLayer*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }
  
  return args.This();
}

Handle<Value> MSLayer::New(layerObj *layer) {
  return ClosedPtr<MSLayer, layerObj>::Closed(layer);
}

Handle<Value> MSLayer::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "name") == 0) {
    RETURN_STRING(layer->this_->name);
  }
  return Undefined();
}

void MSLayer::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "name") == 0) {
    REPLACE_STRING(layer->this_->name, value)
  }
}