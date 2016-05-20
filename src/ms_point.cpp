#include "ms_point.hpp"

Nan::Persistent<v8::FunctionTemplate> MSPoint::constructor;

void MSPoint::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(MSPoint::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Point").ToLocalChecked());

  RW_ATTR(tpl, "x", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "y", PropertyGetter, PropertySetter);

  Nan::SetPrototypeMethod(tpl, "project", Project);
  Nan::SetPrototypeMethod(tpl, "distanceToPoint", DistanceToPoint);

  target->Set(Nan::New("Point").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSPoint::MSPoint(pointObj *point) : ObjectWrap(), this_(point) {
  owner = false;
}

MSPoint::MSPoint() : ObjectWrap(), this_(0) { owner = false; }

MSPoint::~MSPoint() {
  if (this_ && owner) {
    free(this_);
    owner = false;
  }
}

NAN_METHOD(MSPoint::New) {
  MSPoint *obj;

  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void* ptr = ext->Value();
    obj = static_cast<MSPoint*>(ptr);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  pointObj *point = (pointObj *)calloc(1, sizeof(pointObj));
  if(!point) {
    info.GetReturnValue().Set(info.This());
    return;
  }

  double x = -1;
  double y = -1;


  if (info.Length() == 1) {
    Nan::ThrowTypeError("Points take numeric 2 arguments, x and y.");
    return;
  }

  if (info.Length() == 2) {
    if (info[0]->IsNumber() && info[1]->IsNumber()) {
      x = info[0]->NumberValue();
      y = info[1]->NumberValue();
    } else {
      Nan::ThrowTypeError("Points take numeric 2 arguments, x and y.");
      return;
    }
  }

  point->x = x;
  point->y = y;

  obj = new MSPoint(point);
  obj->owner = true;
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSPoint::NewInstance(pointObj *ptr) {
  Nan::EscapableHandleScope scope;
  MSPoint* obj = new MSPoint();
  obj->this_ = ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_GETTER(MSPoint::PropertyGetter) {
  MSPoint *obj = Nan::ObjectWrap::Unwrap<MSPoint>(info.Holder());

  if (STRCMP(property, "x")) {
    info.GetReturnValue().Set(obj->this_->x);
  } else if (STRCMP(property, "y")) {
    info.GetReturnValue().Set(obj->this_->y);
  }
}

NAN_SETTER(MSPoint::PropertySetter) {
  MSPoint *obj = Nan::ObjectWrap::Unwrap<MSPoint>(info.Holder());
  if (STRCMP(property, "x")) {
    obj->this_->x = value->NumberValue();
  } else if (STRCMP(property, "y")) {
    obj->this_->y = value->NumberValue();
  }
}

NAN_METHOD(MSPoint::Project) {
  MSPoint *point = Nan::ObjectWrap::Unwrap<MSPoint>(info.Holder());

  v8::Local<v8::Object> obj;
  MSProjection *projIn;
  MSProjection *projOut;

  if (info.Length() != 2) {
    Nan::ThrowError("projecting a point requires two projection arguments");
    return;
  }

  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("first argument to project must be Projection object");
    return;
  }

  obj = info[0].As<v8::Object>();

  if (obj->IsNull() || obj->IsUndefined() || !Nan::New(MSProjection::constructor)->HasInstance(obj)) {
    Nan::ThrowTypeError("first argument to project must be Projection object");
    return;
  }

  projIn = Nan::ObjectWrap::Unwrap<MSProjection>(obj);

  if (!info[1]->IsObject()) {
    Nan::ThrowTypeError("second argument to project must be Projection object");
    return;
  }

  obj = info[1].As<v8::Object>();


  if (obj->IsNull() || obj->IsUndefined() || !Nan::New(MSProjection::constructor)->HasInstance(obj)) {
    Nan::ThrowTypeError("second argument to project must be Projection object");
    return;
  }

  projOut = Nan::ObjectWrap::Unwrap<MSProjection>(obj);

  msProjectPoint(projIn->this_, projOut->this_, point->this_);
}

NAN_METHOD(MSPoint::DistanceToPoint) {
  MSPoint *point = Nan::ObjectWrap::Unwrap<MSPoint>(info.Holder());

  v8::Local<v8::Object> obj;
  MSPoint *anotherPoint;

  if (info.Length() != 1) {
    Nan::ThrowError("distanceToPoint needs a Point to measure to");
  }

  obj = info[0].As<v8::Object>();

  if (!obj->IsObject()) {
    Nan::ThrowTypeError("distanceToPoint argument must be Point object");
  }


  if (obj->IsNull() || obj->IsUndefined() /*|| !Nan::New(MSPoint::constructor)->HasInstance(obj)*/) {
    Nan::ThrowTypeError("distanceToPoint argument must be Point object");
  }

  anotherPoint = Nan::ObjectWrap::Unwrap<MSPoint>(obj);

  int distance = msDistancePointToPoint(point->this_, anotherPoint->this_);
  info.GetReturnValue().Set(distance);
}