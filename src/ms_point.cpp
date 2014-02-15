#include "ms_point.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSPoint::constructor;

void MSPoint::Initialize(Handle<Object> target) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSPoint::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Point"));

  RW_PROPERTY(constructor, "x", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "y", PropertyGetter, PropertySetter);

  NODE_SET_PROTOTYPE_METHOD(constructor, "project", Project);
  NODE_SET_PROTOTYPE_METHOD(constructor, "distanceToPoint", DistanceToPoint);

  target->Set(String::NewSymbol("Point"), constructor->GetFunction());
}

MSPoint::MSPoint(pointObj *point) : ObjectWrap(), this_(point) {
  this->owner = false;
}

MSPoint::MSPoint() : ObjectWrap(), this_(0) {}

MSPoint::~MSPoint() {
  if (this_ && this->owner) {
    free(this_);
  }
}

Handle<Value> MSPoint::New(const Arguments &args) {
  HandleScope scope;
  MSPoint *obj;

  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSPoint*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }

  pointObj *point = (pointObj *)calloc(1, sizeof(pointObj));
  if(!point) {
    return args.This();
  }

  if (args.Length() == 1) {
    THROW_ERROR(Error, "Points take 2 arguments, x and y.");
  }

  if (args.Length() == 2) {
    REQ_DOUBLE_ARG(0, x);
    REQ_DOUBLE_ARG(1, y);
    point->x = x;
    point->y = y;
  } else {
    point->x = -1;
    point->y = -1;
  }

  obj = new MSPoint(point);
  obj->owner = true;
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> MSPoint::New(pointObj *point) {
  return ClosedPtr<MSPoint, pointObj>::Closed(point);
}

Handle<Value> MSPoint::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSPoint *point = ObjectWrap::Unwrap<MSPoint>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "x") == 0) {
    RETURN_NUMBER(point->this_->x);
  } else if (strcmp(*n, "y") == 0) {
    RETURN_NUMBER(point->this_->y);
  }
  return Undefined();
}

void MSPoint::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSPoint *point = ObjectWrap::Unwrap<MSPoint>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "x") == 0) {
    point->this_->x = value->NumberValue();
  } else if (strcmp(*n, "y") == 0) {
    point->this_->y = value->NumberValue();
  }
}

Handle<Value> MSPoint::Project(const Arguments &args) {
  MSPoint *point = ObjectWrap::Unwrap<MSPoint>(args.This());
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
    THROW_ERROR(TypeError, "second argument to project must be Projection object");
  }

  projOut = ObjectWrap::Unwrap<MSProjection>(obj);

  msProjectPoint(projIn->this_, projOut->this_, point->this_);

  return Undefined();
}

Handle<Value> MSPoint::DistanceToPoint(const Arguments &args) {
  MSPoint *point = ObjectWrap::Unwrap<MSPoint>(args.This());
  Local<Object> obj;
  MSPoint *anotherPoint;

  if (args.Length() != 1) {
    THROW_ERROR(Error, "distanceToPoint needs a Point to measure to");
  }

  if (!args[0]->IsObject()) {
    THROW_ERROR(TypeError, "distanceToPoint argument must be Point object");
  }

  obj = args[0]->ToObject();

  if (obj->IsNull() || obj->IsUndefined() || !MSPoint::constructor->HasInstance(obj)) {
    THROW_ERROR(TypeError, "distanceToPoint argument must be Point object");
  }

  anotherPoint = ObjectWrap::Unwrap<MSPoint>(obj);

  int distance = msDistancePointToPoint(point->this_, anotherPoint->this_);
  RETURN_NUMBER(distance);
}