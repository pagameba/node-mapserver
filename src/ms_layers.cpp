#include "ms_layers.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSLayers::constructor;

void MSLayers::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSLayers::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Layers"));
  
  constructor->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, NULL, NULL, NULL, NULL);
  constructor->InstanceTemplate()->SetNamedPropertyHandler(NamedGetter, NULL, NULL, NULL, NULL);
  
}

MSLayers::MSLayers(mapObj *map) : ObjectWrap(), this_(map) {}

MSLayers::MSLayers() : ObjectWrap(), this_(0) {}

MSLayers::~MSLayers() { }

Handle<Value> MSLayers::New(const Arguments &args) {
  HandleScope scope;
  MSLayers *obj;
  
  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSLayers*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }
  
  return args.This();
}

Handle<Value> MSLayers::New(mapObj *map) {
  return ClosedPtr<MSLayers, mapObj>::Closed(map);
}

Handle<Value> MSLayers::IndexGetter(uint32_t index, const AccessorInfo& info) {
  MSLayers *layers = ObjectWrap::Unwrap<MSLayers>(info.This());
  
  if (index >=0 && index < layers->this_->numlayers) {
    HandleScope scope;
    return scope.Close(MSLayer::New(GET_LAYER(layers->this_, index)));
  }

  return Undefined();
}

Handle<Value> MSLayers::NamedGetter (Local<String> property, const AccessorInfo& info) {
  MSLayers *layers = ObjectWrap::Unwrap<MSLayers>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "length") == 0) {
    RETURN_NUMBER(layers->this_->numlayers);
  } else {
    int i;
    for (i=0; i<layers->this_->numlayers; i++) {
      if (strcmp(*n, GET_LAYER(layers->this_, i)->name) == 0) {
        HandleScope scope;
        return scope.Close(MSLayer::New(GET_LAYER(layers->this_, i)));
      }
    }
  }
  return Undefined();
}


