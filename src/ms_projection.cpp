#include "ms_projection.hpp"

Nan::Persistent<v8::FunctionTemplate> MSProjection::constructor;

void MSProjection::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(MSProjection::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Projection").ToLocalChecked());

  RO_ATTR(tpl, "units", Units);
  RW_ATTR(tpl, "projString", ProjString, SetProjString);

  target->Set(Nan::New("Projection").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSProjection::MSProjection(projectionObj *proj) : ObjectWrap(), this_(proj) {
  owner = false;
}

MSProjection::MSProjection() : ObjectWrap(), this_(0) { owner = false; }

MSProjection::~MSProjection() {
  if (this_ && owner == true) {
    msFreeProjection(this_);
    free(this_);
    owner = false;
  }
}

NAN_METHOD(MSProjection::New) {
  MSProjection *obj;

  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
    return;
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void* ptr = ext->Value();
    obj = static_cast<MSProjection*>(ptr);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  if (info.Length() > 1 || info.Length() < 1 || !info[0]->IsString()) {
    Nan::ThrowTypeError("requires one argument: a projection definition string");
    return;
  }

  int status;
  projectionObj *proj=NULL;

  proj = (projectionObj *)malloc(sizeof(projectionObj));
  if(!proj) {
    info.GetReturnValue().Set(info.This());
    return;
  }
  msInitProjection(proj);

  status = msLoadProjectionString(proj, TOSTR(info[0]));
  if(status == -1) {
    msFreeProjection(proj);
    free(proj);
    info.GetReturnValue().Set(info.This());
    return;
  }

  obj = new MSProjection(proj);
  obj->owner = true;
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSProjection::NewInstance(projectionObj *ptr) {
  Nan::EscapableHandleScope scope;
  MSProjection* obj = new MSProjection();
  obj->this_ = ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_GETTER(MSProjection::Units) {
  MSProjection *proj = Nan::ObjectWrap::Unwrap<MSProjection>(info.Holder());
  int units = GetMapserverUnitUsingProj(proj->this_);
  info.GetReturnValue().Set(units);
}

NAN_GETTER(MSProjection::ProjString) {
  MSProjection *proj = Nan::ObjectWrap::Unwrap<MSProjection>(info.Holder());
  info.GetReturnValue().Set(Nan::New(msGetProjectionString(proj->this_)).ToLocalChecked());
}

NAN_SETTER(MSProjection::SetProjString) {
  MSProjection *proj = Nan::ObjectWrap::Unwrap<MSProjection>(info.Holder());
  if (!value->IsString())
  {
      Nan::ThrowTypeError("projection string value must be a string");
      return;
  }
  msLoadProjectionString(proj->this_, TOSTR(value));
}
