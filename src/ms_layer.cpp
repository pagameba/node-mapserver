#include "ms_layer.hpp"
#include "ms_common.hpp"
#include "ms_map.hpp"

Persistent<FunctionTemplate> MSLayer::constructor;

void MSLayer::Initialize(Handle<Object> target) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSLayer::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Layer"));

  NODE_SET_PROTOTYPE_METHOD(constructor, "getGridIntersectionCoordinates", GetGridIntersectionCoordinates);
  NODE_SET_PROTOTYPE_METHOD(constructor, "updateFromString", UpdateFromString);

  RW_PROPERTY(constructor, "name", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "status", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "type", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "connection", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "minscaledenom", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "maxscaledenom", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "projection", PropertyGetter, PropertySetter);

  RO_PROPERTY(constructor, "connectiontype", PropertyGetter);
  RO_PROPERTY(constructor, "metadata", PropertyGetter);

  target->Set(String::NewSymbol("Layer"), constructor->GetFunction());
}

MSLayer::MSLayer(layerObj *layer) : ObjectWrap(), this_(layer) {}

MSLayer::MSLayer() : ObjectWrap(), this_(0) {}

MSLayer::~MSLayer() { }

Handle<Value> MSLayer::New(const Arguments &args) {
  HandleScope scope;
  layerObj *layer;
  MSLayer *obj;

  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSLayer*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }

  REQ_STR_ARG(0, layer_name);

  layer = (layerObj*)calloc(1,sizeof(layerObj));
  initLayer(layer, (mapObj*)NULL);

  layer->name = strdup(*layer_name);

  obj = new MSLayer(layer);
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> MSLayer::New(layerObj *layer) {
  return ClosedPtr<MSLayer, layerObj>::Closed(layer);
}

Handle<Value> MSLayer::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "name") == 0) {
    RETURN_STRING(layer->this_->name);
  } else if (strcmp(*n, "status") == 0) {
    RETURN_NUMBER(layer->this_->status);
  } else if (strcmp(*n, "metadata") == 0) {
    HandleScope scope;
    return scope.Close(MSHashTable::New(&(layer->this_->metadata)));
  } else if (strcmp(*n, "type") == 0) {
    RETURN_NUMBER(layer->this_->type);
  } else if (strcmp(*n, "minscaledenom") == 0) {
    RETURN_NUMBER(layer->this_->minscaledenom);
  } else if (strcmp(*n, "maxscaledenom") == 0) {
    RETURN_NUMBER(layer->this_->maxscaledenom);
  } else if (strcmp(*n, "projection") == 0) {
    HandleScope scope;
    return scope.Close(MSProjection::New(&layer->this_->projection));
  } else if (strcmp(*n, "connection") == 0) {
    if (layer->this_->connection == NULL) {
      return Undefined();
    }
    RETURN_STRING(layer->this_->connection);
  } else if (strcmp(*n, "connectiontype") == 0) {
    if (layer->this_->connectiontype == NULL) {
      return Undefined();
    }
    RETURN_NUMBER(layer->this_->connectiontype);
  } return Undefined();
}

void MSLayer::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "name") == 0) {
    REPLACE_STRING(layer->this_->name, value)
  } else if (strcmp(*n, "status") == 0) {
    layer->this_->status = value->NumberValue();
  } else if (strcmp(*n, "minscaledenom") == 0) {
    layer->this_->minscaledenom = value->NumberValue();
  } else if (strcmp(*n, "maxscaledenom") == 0) {
    layer->this_->maxscaledenom = value->NumberValue();
  } else if (strcmp(*n, "projection") == 0) {
    v8::String::AsciiValue _v_(value->ToString());
    msLoadProjectionString(&(layer->this_->projection), *_v_);
  } else if (strcmp(*n, "type") == 0) {
    int32_t type = value->Int32Value();
    if (type >= MS_LAYER_ANNOTATION && type <= MS_LAYER_TILEINDEX) {
      layer->this_->type = (MS_LAYER_TYPE) type;
    }
  } else if (strcmp(*n, "connection") == 0) {
    REPLACE_STRING(layer->this_->connection, value)
  }
}

Handle<Value> MSLayer::GetGridIntersectionCoordinates (const Arguments& args) {
  HandleScope scope;
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(args.This());

  int i = 0;

  graticuleIntersectionObj *values = msGraticuleLayerGetIntersectionPoints(layer->this_->map, layer->this_);

  Handle<ObjectTemplate> objTempl = ObjectTemplate::New();
  objTempl->SetInternalFieldCount(1);

  Handle<Array> left = Array::New(values->nLeft);
  Handle<Array> top = Array::New(values->nTop);
  Handle<Array> right = Array::New(values->nRight);
  Handle<Array> bottom = Array::New(values->nBottom);

  for (i=0; i<values->nLeft; i++) {
    Local<Object> val = objTempl->NewInstance();
    val->Set(String::New("x"), Number::New(values->pasLeft[i].x));
    val->Set(String::New("y"), Number::New(values->pasLeft[i].y));
    val->Set(String::New("label"), String::New(values->papszLeftLabels[i]));
    left->Set(i, val);
  }
  for (i=0; i<values->nTop; i++) {
    Local<Object> val = objTempl->NewInstance();
    val->Set(String::New("x"), Number::New(values->pasTop[i].x));
    val->Set(String::New("y"), Number::New(values->pasTop[i].y));
    val->Set(String::New("label"), String::New(values->papszTopLabels[i]));
    top->Set(i, val);
  }
  for (i=0; i<values->nRight; i++) {
    Local<Object> val = objTempl->NewInstance();
    val->Set(String::New("x"), Number::New(values->pasRight[i].x));
    val->Set(String::New("y"), Number::New(values->pasRight[i].y));
    val->Set(String::New("label"), String::New(values->papszRightLabels[i]));
    right->Set(i, val);
  }
  for (i=0; i<values->nBottom; i++) {
    Local<Object> val = objTempl->NewInstance();
    val->Set(String::New("x"), Number::New(values->pasBottom[i].x));
    val->Set(String::New("y"), Number::New(values->pasBottom[i].y));
    val->Set(String::New("label"), String::New(values->papszBottomLabels[i]));
    bottom->Set(i, val);
  }

  // return object like this:
  // {
  //   left: [{position: 0, label: '123.00'}],
  //   top: [{position: 0, label: '123.00'}],
  //   right: [{position: 0, label: '123.00'}],
  //   bottom: [{position: 0, label: '123.00'}],
  // }
  Local<Object> result = objTempl->NewInstance();
  result->Set(String::New("left"), left);
  result->Set(String::New("top"), top);
  result->Set(String::New("right"), right);
  result->Set(String::New("bottom"), bottom);
  return scope.Close(result);
}

Handle<Value> MSLayer::UpdateFromString (const Arguments& args) {
  HandleScope scope;
  int result;
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(args.This());
  REQ_STR_ARG(0, snippet);
  result = msUpdateLayerFromString(layer->this_, *snippet, MS_FALSE);
  return scope.Close(Number::New(result));
}

