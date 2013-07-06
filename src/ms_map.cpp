#include "ms_map.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSMap::constructor;

void MSMap::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSMap::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Map"));
  
  // NODE_SET_PROTOTYPE_METHOD(constructor, "selectOutputFormat", SelectOutputFormat);
  NODE_SET_PROTOTYPE_METHOD(constructor, "setExtent", SetExtent);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "drawMap", DrawMap);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "recompute", Recompute);
  // NODE_SET_PROTOTYPE_METHOD(constructor, "copy", Copy);
  
  /* Read-Write Properties */
  RW_PROPERTY(constructor, "name", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "status", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "width", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "height", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "maxsize", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "units", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "resolution", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "defresolution", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "shapepath", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "mappath", PropertyGetter, PropertySetter);
  RW_PROPERTY(constructor, "imagetype", PropertyGetter, PropertySetter);

  /* Read-Only Properties */
  RO_PROPERTY(constructor, "cellsize", PropertyGetter);
  RO_PROPERTY(constructor, "scaledenom", PropertyGetter);
  RO_PROPERTY(constructor, "mimetype", PropertyGetter);
  
  RO_PROPERTY(constructor, "extent", PropertyGetter);
  
  target->Set(String::NewSymbol("Map"), constructor->GetFunction());
}

MSMap::MSMap(mapObj *map) : ObjectWrap(), this_(map) {}

MSMap::MSMap() : ObjectWrap(), this_(0) {}

MSMap::~MSMap() { 
  if (this_) {
    msFreeMap(this_);
  }
}

Handle<Value> MSMap::New(const Arguments &args) {
  HandleScope scope;

  if (!args.IsConstructCall())
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    MSMap *f =  static_cast<MSMap *>(ptr);
    f->Wrap(args.This());
    return args.This();
  }

  return args.This();
}

Handle<Value> MSMap::New(mapObj *map) {
  return ClosedPtr<MSMap, mapObj>::Closed(map);
}

Handle<Value> MSMap::SetExtent(const Arguments &args) {
  HandleScope scope;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  REQ_DOUBLE_ARG(0, minx);
  REQ_DOUBLE_ARG(1, miny);
  REQ_DOUBLE_ARG(2, maxx);
  REQ_DOUBLE_ARG(3, maxy);
  map->this_->extent.minx = minx;
  map->this_->extent.miny = miny;
  map->this_->extent.maxx = maxx;
  map->this_->extent.maxy = maxy;
  return scope.Close(Boolean::New(true));
}

Handle<Value> MSMap::PropertyGetter (Local<String> property, const AccessorInfo& info) {
  MSMap *map = ObjectWrap::Unwrap<MSMap>(info.This());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "width") == 0) {
    RETURN_NUMBER(map->this_->width);
  } else if (strcmp(*n, "height") == 0) {
    RETURN_NUMBER(map->this_->height);
  } else if (strcmp(*n, "status") == 0) {
    RETURN_NUMBER(map->this_->status);
  } else if (strcmp(*n, "maxsize") == 0) {
    RETURN_NUMBER(map->this_->maxsize);
  } else if (strcmp(*n, "cellsize") == 0) {
    RETURN_NUMBER(map->this_->cellsize);
  } else if (strcmp(*n, "units") == 0) {
    RETURN_NUMBER(map->this_->units);
  } else if (strcmp(*n, "scaledenom") == 0) {
    RETURN_NUMBER(map->this_->scaledenom);
  } else if (strcmp(*n, "resolution") == 0) {
    RETURN_NUMBER(map->this_->resolution);
  } else if (strcmp(*n, "defresolution") == 0) {
    RETURN_NUMBER(map->this_->defresolution);
  } else if (strcmp(*n, "imagetype") == 0) {
    RETURN_STRING(map->this_->imagetype);
  } else if (strcmp(*n, "mimetype") == 0) {
    RETURN_STRING(map->this_->outputformat->mimetype);
  } else if (strcmp(*n, "shapepath") == 0) {
    RETURN_STRING(map->this_->shapepath);
  } else if (strcmp(*n, "mappath") == 0) {
    RETURN_STRING(map->this_->mappath);
  } else if (strcmp(*n, "name") == 0) {
    RETURN_STRING(map->this_->name);
  } else if (strcmp(*n, "outputformat") == 0) {
    // if (outputformat_template_.IsEmpty()) {
    //   Handle<ObjectTemplate> raw_template = ObjectTemplate::New();
    //   raw_template->SetInternalFieldCount(1);
    //   raw_template->SetNamedPropertyHandler(OutputFormatNamedGetter, NULL, NULL, NULL, NULL);
    //   outputformat_template_ = Persistent<ObjectTemplate>::New(raw_template);
    // }
    // Handle<ObjectTemplate> templ = outputformat_template_;
    // Handle<Object> result = templ->NewInstance();
    // Handle<External> map_ptr = External::New(map);
    // result->SetInternalField(0,map_ptr);
    // HandleScope scope;
    // return scope.Close(result);
  } else if (strcmp(*n, "layers") == 0) {
    // if (layers_template_.IsEmpty()) {
    //   Handle<ObjectTemplate> raw_template = ObjectTemplate::New();
    //   raw_template->SetInternalFieldCount(1);
    //   raw_template->SetIndexedPropertyHandler(LayersIndexedGetter, NULL, NULL, NULL, NULL);
    //   raw_template->SetNamedPropertyHandler(LayersNamedGetter, NULL, NULL, NULL, NULL);
    //   layers_template_ = Persistent<ObjectTemplate>::New(raw_template);
    // }
    // Handle<ObjectTemplate> templ = layers_template_;
    // Handle<Object> result = templ->NewInstance();
    // Handle<External> map_ptr = External::New(map);
    // result->SetInternalField(0,map_ptr);
    // HandleScope scope;
    // return scope.Close(result);
  } else if (strcmp(*n, "extent") == 0) {
    HandleScope scope;
    return scope.Close(MSRect::New(&map->this_->extent));
    // if (extent_template_.IsEmpty()) {
    //   Handle<ObjectTemplate> raw_template = ObjectTemplate::New();
    //   raw_template->SetInternalFieldCount(1);
    //   raw_template->SetNamedPropertyHandler(ExtentNamedGetter, NULL, NULL, NULL, NULL);
    //   extent_template_ = Persistent<ObjectTemplate>::New(raw_template);
    // }
    // Handle<ObjectTemplate> templ = extent_template_;
    // Handle<Object> result = templ->NewInstance();
    // Handle<External> map_ptr = External::New(map);
    // result->SetInternalField(0,map_ptr);
    // HandleScope scope;
    // return scope.Close(result);
  }
  return Undefined();
}

void MSMap::PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSMap *map = ObjectWrap::Unwrap<MSMap>(info.Holder());
  v8::String::AsciiValue n(property);
  if (strcmp(*n, "width") == 0) {
    map->this_->width = value->Int32Value();
  } else if (strcmp(*n, "height") == 0) {
    map->this_->height = value->Int32Value();
  } else if (strcmp(*n, "maxsize") == 0) {
    map->this_->maxsize = value->Int32Value();
  } else if (strcmp(*n, "units") == 0) {
    int32_t units = value->Int32Value();
    if (units >= MS_INCHES && units <= MS_NAUTICALMILES) {
      map->this_->units = (MS_UNITS) units;
    }
  } else if (strcmp(*n, "resolution") == 0) {
    map->this_->resolution = value->NumberValue();
  } else if (strcmp(*n, "defresolution") == 0) {
    map->this_->defresolution = value->NumberValue();
  } else if (strcmp(*n, "name") == 0) {
    REPLACE_STRING(map->this_->name, value);
  } else if (strcmp(*n, "imagetype") == 0) {
    REPLACE_STRING(map->this_->imagetype, value);
  } else if (strcmp(*n, "shapepath") == 0) {
    REPLACE_STRING(map->this_->shapepath, value);
  } else if (strcmp(*n, "mappath") == 0) {
    REPLACE_STRING(map->this_->mappath, value);
  }
}