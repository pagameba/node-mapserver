#include "ms_layer.hpp"
#include "ms_hashtable.hpp"
#include "ms_projection.hpp"

Nan::Persistent<v8::FunctionTemplate> MSLayer::constructor;

void MSLayer::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New <v8::FunctionTemplate>(MSLayer::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Layer").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "getGridIntersectionCoordinates", GetGridIntersectionCoordinates);
  Nan::SetPrototypeMethod(tpl, "updateFromString", UpdateFromString);
#if MS_VERSION_NUM >= 60400
  Nan::SetPrototypeMethod(tpl, "toString", ToString);
#endif

  RW_ATTR(tpl, "name", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "status", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "type", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "connection", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "minscaledenom", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "maxscaledenom", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "projection", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "units", PropertyGetter, PropertySetter);
  RO_ATTR(tpl, "connectiontype", PropertyGetter);
  RO_ATTR(tpl, "metadata", PropertyGetter);

  target->Set(Nan::New("Layer").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSLayer::MSLayer(layerObj *layer) : ObjectWrap(), this_(layer) {}

MSLayer::MSLayer() : ObjectWrap(), this_(0) {}

MSLayer::~MSLayer() { }

NAN_METHOD(MSLayer::New)
{
  MSLayer* obj;
  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
    return;
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void *ptr = ext->Value();
    MSLayer *f =  static_cast<MSLayer *>(ptr);
    f->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  if (info.Length() != 1 || !info[0]->IsString()) {
    Nan::ThrowTypeError("requires one argument: a string");
    return;
  }

  layerObj* layer = (layerObj*)calloc(1,sizeof(layerObj));
  initLayer(layer, (mapObj*)NULL);

  layer->name = strdup(TOSTR(info[0]));

  obj = new MSLayer(layer);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSLayer::NewInstance(layerObj *ptr) {
  Nan::EscapableHandleScope scope;
  MSLayer* obj = new MSLayer();
  obj->this_ = ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_GETTER(MSLayer::PropertyGetter) {

  MSLayer *obj = Nan::ObjectWrap::Unwrap<MSLayer>(info.Holder());

  if (STRCMP(property, "name")) {
    info.GetReturnValue().Set(Nan::New(obj->this_->name).ToLocalChecked());
  } else if (STRCMP(property, "status")) {
    info.GetReturnValue().Set(obj->this_->status);
  } else if (STRCMP(property, "metadata")) {
    info.GetReturnValue().Set(MSHashTable::NewInstance(&(obj->this_->metadata)));
  } else if (STRCMP(property, "type")) {
    info.GetReturnValue().Set(obj->this_->type);
  } else if (STRCMP(property, "minscaledenom")) {
    info.GetReturnValue().Set(obj->this_->minscaledenom);
  } else if (STRCMP(property, "maxscaledenom")) {
    info.GetReturnValue().Set(obj->this_->maxscaledenom);
  } else if (STRCMP(property, "units")) {
    info.GetReturnValue().Set(obj->this_->units);
  } else if (STRCMP(property, "projection")) {
    info.GetReturnValue().Set(MSProjection::NewInstance(&obj->this_->projection));
  } else if (STRCMP(property, "connection")) {
    if (obj->this_->connection == NULL) {
      info.GetReturnValue().Set(Nan::Undefined());
    } else {
      info.GetReturnValue().Set(Nan::New(obj->this_->connection).ToLocalChecked());
    }
  } else if (STRCMP(property, "connectiontype")) {
    info.GetReturnValue().Set(obj->this_->connectiontype);
  }
}

NAN_SETTER(MSLayer::PropertySetter) {
  MSLayer *obj = Nan::ObjectWrap::Unwrap<MSLayer>(info.Holder());

  if (STRCMP(property, "name")) {
    REPLACE_STRING(obj->this_->name, value)
  } else if (STRCMP(property, "status")) {
    obj->this_->status = value->NumberValue();
  } else if (STRCMP(property, "minscaledenom")) {
    obj->this_->minscaledenom = value->NumberValue();
  } else if (STRCMP(property, "maxscaledenom")) {
    obj->this_->maxscaledenom = value->NumberValue();
  } else if (STRCMP(property, "units")) {
    int32_t units = value->Int32Value();
    if (units >= MS_INCHES && units <= MS_NAUTICALMILES) {
      obj->this_->units = (MS_UNITS) units;
    }
  } else if (STRCMP(property, "projection")) {
    msLoadProjectionString(&(obj->this_->projection), TOSTR(value));
  } else if (STRCMP(property, "type")) {
    int32_t type = value->Int32Value();
    if (type >= MS_LAYER_ANNOTATION && type <= MS_LAYER_TILEINDEX) {
      obj->this_->type = (MS_LAYER_TYPE) type;
    }
  } else if (STRCMP(property, "connection")) {
    REPLACE_STRING(obj->this_->connection, value)
  }
}

NAN_METHOD(MSLayer::GetGridIntersectionCoordinates) {
  MSLayer *obj = Nan::ObjectWrap::Unwrap<MSLayer>(info.This());

  int i = 0;

  graticuleIntersectionObj *values = msGraticuleLayerGetIntersectionPoints(obj->this_->map, obj->this_);

  v8::Local<v8::Array> left = Nan::New<v8::Array>(values->nLeft);
  v8::Local<v8::Array> top = Nan::New<v8::Array>(values->nTop);
  v8::Local<v8::Array> right = Nan::New<v8::Array>(values->nRight);
  v8::Local<v8::Array> bottom = Nan::New<v8::Array>(values->nBottom);
  v8::Local<v8::Object> val;

  for (i=0; i<values->nLeft; i++) {
    val = Nan::New<v8::Object>();
    val->Set(Nan::New("x").ToLocalChecked(), Nan::New(values->pasLeft[i].x));
    val->Set(Nan::New("y").ToLocalChecked(), Nan::New(values->pasLeft[i].y));
    val->Set(Nan::New("label").ToLocalChecked(), Nan::New(values->papszLeftLabels[i]).ToLocalChecked());
    left->Set(i, val);
  }
  for (i=0; i<values->nTop; i++) {
    val = Nan::New<v8::Object>();
    val->Set(Nan::New("x").ToLocalChecked(), Nan::New(values->pasTop[i].x));
    val->Set(Nan::New("y").ToLocalChecked(), Nan::New(values->pasTop[i].y));
    val->Set(Nan::New("label").ToLocalChecked(), Nan::New(values->papszTopLabels[i]).ToLocalChecked());
    top->Set(i, val);
  }
  for (i=0; i<values->nRight; i++) {
    val = Nan::New<v8::Object>();
    val->Set(Nan::New("x").ToLocalChecked(), Nan::New(values->pasRight[i].x));
    val->Set(Nan::New("y").ToLocalChecked(), Nan::New(values->pasRight[i].y));
    val->Set(Nan::New("label").ToLocalChecked(), Nan::New(values->papszRightLabels[i]).ToLocalChecked());
    right->Set(i, val);
  }
  for (i=0; i<values->nBottom; i++) {
    val = Nan::New<v8::Object>();
    val->Set(Nan::New("x").ToLocalChecked(), Nan::New(values->pasBottom[i].x));
    val->Set(Nan::New("y").ToLocalChecked(), Nan::New(values->pasBottom[i].y));
    val->Set(Nan::New("label").ToLocalChecked(), Nan::New(values->papszBottomLabels[i]).ToLocalChecked());
    bottom->Set(i, val);
  }

  // return object like this:
  // {
  //   left: [{position: 0, label: '123.00'}],
  //   top: [{position: 0, label: '123.00'}],
  //   right: [{position: 0, label: '123.00'}],
  //   bottom: [{position: 0, label: '123.00'}],
  // }
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  result->Set(Nan::New("left").ToLocalChecked(), left);
  result->Set(Nan::New("top").ToLocalChecked(), top);
  result->Set(Nan::New("right").ToLocalChecked(), right);
  result->Set(Nan::New("bottom").ToLocalChecked(), bottom);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(MSLayer::UpdateFromString) {
  MSLayer *obj = Nan::ObjectWrap::Unwrap<MSLayer>(info.This());
  int result;
  if (info.Length() < 1) {
    Nan::ThrowError("UpdateFromString requires one string argument");
    return;
  }
  if (!info[0]->IsString()) {
    Nan::ThrowError("UpdateFromString requires one string argument");
    return;
  }
  result = msUpdateLayerFromString(obj->this_, TOSTR(info[0]), MS_FALSE);
  info.GetReturnValue().Set(result);
}

#if MS_VERSION_NUM >= 60400
NAN_METHOD(MSLayer::ToString) {
  MSLayer *obj = Nan::ObjectWrap::Unwrap<MSLayer>(info.This());
  char *text = msWriteLayerToString(obj->this_);
  info.GetReturnValue().Set(Nan::New(text).ToLocalChecked());
}
#endif
