#include "ms_layer.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSLayer::constructor;

void MSLayer::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSLayer::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Layer"));
  
  NODE_SET_PROTOTYPE_METHOD(constructor, "getGridIntersectionCoordinates", GetGridIntersectionCoordinates);
  RW_PROPERTY(constructor, "name", PropertyGetter, PropertySetter);
  
  target->Set(String::NewSymbol("Layer"), constructor->GetFunction());
}

MSLayer::MSLayer(layerObj *layer) : ObjectWrap(), this_(layer) {}

MSLayer::MSLayer() : ObjectWrap(), this_(0) {}

MSLayer::~MSLayer() { }

Handle<Value> MSLayer::New(const Arguments &args) {
  HandleScope scope;
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
  }
  return Undefined();
}

void MSLayer::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSLayer *layer = ObjectWrap::Unwrap<MSLayer>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "name") == 0) {
    REPLACE_STRING(layer->this_->name, value)
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
