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
  
  NODE_SET_PROTOTYPE_METHOD(constructor, "project", Project);
  
  target->Set(String::NewSymbol("Rect"), constructor->GetFunction());
}

MSRect::MSRect(rectObj *rect) : ObjectWrap(), this_(rect) {}

MSRect::MSRect() : ObjectWrap(), this_(0) {}

MSRect::~MSRect() { }

Handle<Value> MSRect::New(const Arguments &args) {
  HandleScope scope;
  MSRect *obj;
  double t;
  
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
  
  if (args.Length() == 0) {
    rect->minx = -1;
    rect->miny = -1;
    rect->maxx = -1;
    rect->maxy = -1;
  } else if (args.Length() == 4) {
    REQ_DOUBLE_ARG(0, minx);
    REQ_DOUBLE_ARG(1, miny);
    REQ_DOUBLE_ARG(2, maxx);
    REQ_DOUBLE_ARG(3, maxy);
    /* coerce correct extent */
    if (minx > maxx) {
      t = maxx;
      maxx = minx;
      minx = t;
    }
    if (miny > maxy) {
      t = maxy;
      maxy = miny;
      miny = t;
    }
    rect->minx = minx;
    rect->miny = miny;
    rect->maxx = maxx;
    rect->maxy = maxy;
  } else {
    THROW_ERROR(Error, "Rect objects take 0 or 4 arguments.");
  }
  
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
  double t;
  
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

Handle<Value> MSRect::Project(const Arguments &args) {
  MSRect *rect = ObjectWrap::Unwrap<MSRect>(args.This());
  Local<Object> obj;
  MSProjection *projIn;
  MSProjection *projOut;
  
  if (args.Length() != 2) {
    THROW_ERROR(Error, "projecting a point requires two projection arguments");
  }
  
  if (!args[0]->IsObject()) {
    THROW_ERROR(TypeError, "first argument to project must be Projection object");
  }

  obj = args[0]->ToObject();

  if (obj->IsNull() || obj->IsUndefined() || !MSProjection::constructor->HasInstance(obj)) {
    THROW_ERROR(TypeError, "first argument to project must be Projection object");
  }

  projIn = ObjectWrap::Unwrap<MSProjection>(obj);
  
  if (!args[1]->IsObject()) {
    THROW_ERROR(TypeError, "second argument to project must be Projection object");
  }

  obj = args[1]->ToObject();

  if (obj->IsNull() || obj->IsUndefined() || !MSProjection::constructor->HasInstance(obj)) {
    THROW_ERROR(TypeError, "first argument to project must be Projection object");
  }

  projOut = ObjectWrap::Unwrap<MSProjection>(obj);
  
  msProjectRect(projIn->this_, projOut->this_, rect->this_);
    
  return Undefined();
}
