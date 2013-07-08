#include "ms_projection.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSProjection::constructor;

void MSProjection::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSProjection::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Projection"));
  
  RO_PROPERTY(constructor, "units", Units);

  target->Set(String::NewSymbol("Projection"), constructor->GetFunction());
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
  MSProjection *obj;
  
  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSProjection*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }
  
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
  
  obj = new MSProjection(proj);
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> MSProjection::New(projectionObj *proj) {
  return ClosedPtr<MSProjection, projectionObj>::Closed(proj);
}

Handle<Value> MSProjection::Units (Local<String> property, const AccessorInfo& info) {
  MSProjection *proj = ObjectWrap::Unwrap<MSProjection>(info.This());
  HandleScope scope;
  int units = GetMapserverUnitUsingProj(proj->this_);
  Local<Number> result = Number::New(units);
  return scope.Close(result);
}

