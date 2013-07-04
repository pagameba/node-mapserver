#include map.cpp
#include utils.hpp

Persistent<FunctionTemplate> constructor;

static void Map::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Map::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Map"));
  
  NODE_SET_PROTOTYPE_METHOD(constructor, "selectOutputFormat", SelectOutputFormat);
  NODE_SET_PROTOTYPE_METHOD(constructor, "setExtent", SetExtent);
  NODE_SET_PROTOTYPE_METHOD(constructor, "drawMap", DrawMap);
  NODE_SET_PROTOTYPE_METHOD(constructor, "recompute", Recompute);
  NODE_SET_PROTOTYPE_METHOD(constructor, "copy", Copy);
  
  constructor->InstanceTemplate()->SetNamedPropertyHandler(NamedPropertyGetter, NULL, NamedPropertyQuery, NULL, NamedPropertyEnumerator);
  
}

Map::Map(mapObj * map) : ObjectWrap(), _map(map) {}

Map::~Map() { 
  if (_map) {
    msFreeMap(_map);
  }
}


static Handle<Value> New(const Arguments &args) {
  assert(args.IsConstructCall());
  HandleScope scope;
  REQ_EXT_ARG(0, map);
  (new Map((mapObj *)map->Value()))->Wrap(args.This());
  return args.This();
}

static Handle<Value> SelectOutputFormat (const Arguments& args);
static Handle<Value> SetExtent (const Arguments& args);
static Handle<Value> DrawMap (const Arguments& args);
static Handle<Value> Recompute (const Arguments& args);
static Handle<Value> Copy (const Arguments& args);
static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info);
static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
