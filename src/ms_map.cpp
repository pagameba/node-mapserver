#include "ms_map.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSMap::constructor;

void MSMap::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSMap::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("MSMap"));
  
  // NODE_SET_PROTOTYPE_METHOD(constructor, "selectOutputFormat", SelectOutputFormat);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "setExtent", SetExtent);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "drawMap", DrawMap);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "recompute", Recompute);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "copy", Copy);
  
  // constructor->InstanceTemplate()->SetNamedPropertyHandler(NamedPropertyGetter, NULL, NamedPropertyQuery, NULL, NamedPropertyEnumerator);
  
}

MSMap::MSMap(mapObj *map) : ObjectWrap(), this_(map) {}

MSMap::MSMap() : ObjectWrap(), this_(0) {}

MSMap::~MSMap() { 
  if (this_) {
    msFreeMap(this_);
  }
}

Handle<Value> MSMap::New(const Arguments &args) {
  HandleScope scope;

  if (!args.IsConstructCall())
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    MSMap *f =  static_cast<MSMap *>(ptr);
    f->Wrap(args.This());
    return args.This();
  }

  return args.This();
}

Handle<Value> MSMap::New(mapObj *map) {
  return ClosedPtr<MSMap, mapObj>::Closed(map);
}
