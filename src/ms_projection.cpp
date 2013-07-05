#include "ms_projection.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSProjection::constructor;

void MSProjection::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSProjection::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("MSProjection"));
  target->Set(String::NewSymbol("MSProjection"), constructor->GetFunction());
  
  //RO_PROPER(constructor, "units", GetMapUnits);
  constructor->InstanceTemplate()->SetAccessor(String::New("units"),GetMapUnits, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
  
  // NODE_SET_PROTOTYPE_METHOD(constructor, "selectOutputFormat", SelectOutputFormat);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "setExtent", SetExtent);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "drawMap", DrawMap);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "recompute", Recompute);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "copy", Copy);
  
  // constructor->InstanceTemplate()->SetNamedPropertyHandler(NamedPropertyGetter, NULL, NamedPropertyQuery, NULL, NamedPropertyEnumerator);
  
}

MSProjection::MSProjection(projectionObj *proj) : ObjectWrap(), this_(proj) {}

MSProjection::MSProjection() : ObjectWrap(), this_(0) {}

MSProjection::~MSProjection() { 
  if (this_) {
    msFreeProjection(this_);
  }
}

Handle<Value> MSProjection::New(const Arguments &args) {
  HandleScope scope;

  if (!args.IsConstructCall())
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

  REQ_STR_ARG(0, proj_string);

  int status;
  projectionObj *proj=NULL;

  proj = (projectionObj *)malloc(sizeof(projectionObj));
  if(!proj) {
    return args.This();
  }
   
  msInitProjection(proj);

  status = msLoadProjectionString(proj, *proj_string);
  if(status == -1) {
    msFreeProjection(proj);
    free(proj);
    return args.This();
  }
  
  MSProjection *f =  new MSProjection();
  f->this_ = proj;
  f->Wrap(args.This());
  return args.This();
}

Handle<Value> MSProjection::New(projectionObj *proj) {
  return ClosedPtr<MSProjection, projectionObj>::Closed(proj);
}

Handle<Value> MSProjection::GetMapUnits (Local<String> property, const AccessorInfo& info) {
  MSProjection *proj = ObjectWrap::Unwrap<MSProjection>(info.This());
  HandleScope scope;
  int units = GetMapserverUnitUsingProj(proj->this_);
  Local<Number> result = Number::New(units);
  return scope.Close(result);
}

