#include "ms_map.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSMap::constructor;

void MSMap::Initialize(Handle<Object> target) {
  HandleScope scope;
  
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSMap::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Map"));
  
  NODE_SET_PROTOTYPE_METHOD(constructor, "selectOutputFormat", SelectOutputFormat);
  NODE_SET_PROTOTYPE_METHOD(constructor, "setExtent", SetExtent);
  NODE_SET_PROTOTYPE_METHOD(constructor, "drawMap", DrawMap);
  NODE_SET_PROTOTYPE_METHOD(constructor, "recompute", Recompute);
  NODE_SET_PROTOTYPE_METHOD(constructor, "insertLayer", InsertLayer);
  NODE_SET_PROTOTYPE_METHOD(constructor, "setSymbolSet", SetSymbolSet);
  NODE_SET_PROTOTYPE_METHOD(constructor, "save", Save);
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
  RW_PROPERTY(constructor, "projection", PropertyGetter, PropertySetter);

  /* Read-Only Properties */
  RO_PROPERTY(constructor, "cellsize", PropertyGetter);
  RO_PROPERTY(constructor, "scaledenom", PropertyGetter);
  RO_PROPERTY(constructor, "mimetype", PropertyGetter);
  RO_PROPERTY(constructor, "outputformat", PropertyGetter);
  
  RO_PROPERTY(constructor, "extent", PropertyGetter);
  RO_PROPERTY(constructor, "layers", PropertyGetter);
  
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
  mapObj *map;
  MSMap *obj;
  
  if (!args.IsConstructCall())
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    MSMap *f =  static_cast<MSMap *>(ptr);
    f->Wrap(args.This());
    return args.This();
  }
  
  if (args.Length() == 2) {
    REQ_STR_ARG(0, mapfile);
    REQ_STR_ARG(1, mappath);
    map = msLoadMap(*mapfile, *mappath);
  } else if (args.Length() == 1) {
    REQ_STR_ARG(0, mapfile);
    map = msLoadMap(*mapfile, NULL);
  } else {
    map = msNewMapObj();
  }
  
  if (map == NULL) {
    THROW_ERROR(Error, "Unable to load requested map.");
  }

  obj = new MSMap(map);
  obj->Wrap(args.This());
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

Handle<Value> MSMap::Recompute (const Arguments& args) {
  HandleScope scope;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  mapObj * _map = map->this_;
 _map->cellsize = msAdjustExtent(&(_map->extent),
                                   _map->width,
                                   _map->height);
  msCalculateScale(_map->extent,
                   _map->units,
                   _map->width,
                   _map->height,
                   _map->resolution,
                   &_map->scaledenom);
  return scope.Close(Boolean::New(true));
}

Handle<Value> MSMap::InsertLayer (const Arguments& args) {
  HandleScope scope;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  MSLayer *layer;
  Local<Object> obj;
  int result;
  int position = 0;
  
  if (args.Length() < 1) {
    THROW_ERROR(Error, "insertLayer requires at least one argument");
  }
  
  if (!args[0]->IsObject()) {
    THROW_ERROR(TypeError, "first argument to project must be Layer object");
  }

  obj = args[0]->ToObject();

  if (obj->IsNull() || obj->IsUndefined() || !MSLayer::constructor->HasInstance(obj)) {
    THROW_ERROR(TypeError, "first argument to project must be Layer object");
  }

  layer = ObjectWrap::Unwrap<MSLayer>(obj);
  
  if (args.Length() == 2) {
    if (!args[1]->IsNumber()) {
      THROW_ERROR(TypeError, "second argument must be an integer");
    } else {
      position = args[1]->ToInteger()->Value();
      if (position >= map->this_->numlayers) {
        position = map->this_->numlayers-1;
      } else if (position < 0) {
        position = 0;
      }
    }
  }

  result = msInsertLayer(map->this_, layer->this_, position);
  return scope.Close(Number::New(result));
}

Handle<Value> MSMap::SelectOutputFormat (const Arguments& args) {
  HandleScope scope;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  REQ_STR_ARG(0, imagetype);
  outputFormatObj * format = msSelectOutputFormat(map->this_, *imagetype);
  if ( format == NULL) {
    THROW_ERROR(Error, "Output format not supported.");
  }
  msApplyOutputFormat(&(map->this_->outputformat), format, MS_NOOVERRIDE, 
      MS_NOOVERRIDE, MS_NOOVERRIDE );
  return Undefined();
}

Handle<Value> MSMap::SetSymbolSet(const Arguments &args) {
  HandleScope scope;
  int result;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  REQ_STR_ARG(0, symbolfile);

  msFreeSymbolSet(&(map->this_->symbolset));
  msInitSymbolSet(&(map->this_->symbolset));

  // Set symbolset filename
  map->this_->symbolset.filename = strdup(*symbolfile);

  // Symbolset shares same fontset as main mapfile
  map->this_->symbolset.fontset = &(map->this_->fontset);

  result = msLoadSymbolSet(&(map->this_->symbolset), map->this_);
  return scope.Close(Number::New(result));
}

Handle<Value> MSMap::Save(const Arguments &args) {
  HandleScope scope;
  int result;
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  REQ_STR_ARG(0, outputfile);

  result = msSaveMap(map->this_, *outputfile);
  return scope.Close(Number::New(result));
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
    HandleScope scope;  
    return scope.Close(MSOutputFormat::New(map->this_->outputformat));
  } else if (strcmp(*n, "projection") == 0) {
    HandleScope scope;  
    return scope.Close(MSProjection::New(&map->this_->projection));
  } else if (strcmp(*n, "layers") == 0) {
    HandleScope scope;
    return scope.Close(MSLayers::New(map->this_));
  } else if (strcmp(*n, "extent") == 0) {
    HandleScope scope;
    return scope.Close(MSRect::New(&map->this_->extent));
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
  } else if (strcmp(*n, "projection") == 0) {
    v8::String::AsciiValue _v_(value->ToString());
    msLoadProjectionString(&(map->this_->projection), *_v_);
  } else if (strcmp(*n, "mappath") == 0) {
    REPLACE_STRING(map->this_->mappath, value);
  }
}

struct drawmap_baton {
	uv_work_t request;
	MSMap *map;
  errorObj * error;
  int size;
  char * data;
  Persistent<Function> cb;
};

void FreeImageBuffer(char *data, void *hint) {
  msFree(data);
}

void MSMap::DrawMapWork(uv_work_t *req) {
  drawmap_baton *baton = static_cast<drawmap_baton*>(req->data);
  
  imageObj * im = msDrawMap(baton->map->this_, MS_FALSE);
  if (im != NULL) {
    baton->error = NULL;
    baton->data = (char *)msSaveImageBuffer(im, &baton->size, baton->map->this_->outputformat);
    msFreeImage(im);
  } else {
    baton->error = msGetErrorObj();
    baton->data = NULL;
  }
  return;
}

void MSMap::DrawMapAfter(uv_work_t *req) {
  HandleScope scope;
  // drawmap_baton *drawmap_req =(drawmap_baton *)req->data;
  drawmap_baton *baton = static_cast<drawmap_baton *>(req->data);
  baton->map->Unref();

  TryCatch try_catch;

  Local<Value> argv[2];
  if (baton->data != NULL) {
    Buffer * buffer = Buffer::New(baton->data, baton->size, FreeImageBuffer, NULL);

    argv[0] = Local<Value>::New(Null());
    argv[1] = Local<Value>::New(buffer->handle_);
  } else {
    Local<Value> _arg_ = External::New(baton->error);

    // argv[0] = Local<Value>::New(ErrorObj::constructor_template->GetFunction()->NewInstance(1, &_arg_));
    errorObj * err = msGetErrorObj();
    argv[0] = Local<Value>::New(MSError::New(err));
    argv[1] = Local<Value>::New(Null());
  }


  baton->cb->Call(Context::GetCurrent()->Global(), 2, argv);

  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  baton->cb.Dispose();
  delete baton;
  return;
}

/**
 * callback for buffer creation to free the memory assocatiated with the
 * image after its been copied into the buffer
 */

Handle<Value> MSMap::DrawMap (const Arguments& args) {
  HandleScope scope;
  
  REQ_FUN_ARG(0, cb);
  MSMap *map = ObjectWrap::Unwrap<MSMap>(args.This());
  
  char * version = msGetVersion();
  char * regex = "SUPPORTS\\=THREADS";
  int match;
  match = 0;
  if (msEvalRegex(regex, version) == MS_TRUE) {
    match = 1;
  } else {
    // discard the error reported by msEvalRegex saying that it failed
    msResetErrorList();
  }
  
  
  if (match == 1) {
    drawmap_baton * baton = new drawmap_baton();
    baton->request.data = (void*) baton;
    baton->map = map;
    baton->cb = Persistent<Function>::New(cb);
  
    map->Ref();
    uv_queue_work(uv_default_loop(),
      &baton->request,
      DrawMapWork,
      (uv_after_work_cb) DrawMapAfter);
  } else {
    Local<Value> argv[2];
    argv[0] = Local<Value>::New(Null());
    imageObj * im = msDrawMap(map->this_, MS_FALSE);
    if (im != NULL) {
      int size;
      char * data = (char *)msSaveImageBuffer(im, &size, map->this_->outputformat);
      msFreeImage(im);
      Buffer * buffer = Buffer::New(data, size, FreeImageBuffer, NULL);
      
      argv[1] = Local<Value>::New(buffer->handle_);
    } else {
      errorObj * err = msGetErrorObj();
      Local<Value> _arg_ = External::New(err);

      argv[0] = Local<Value>::New(MSError::New(err));
      argv[1] = Local<Value>::New(Null());
    }
    cb->Call(Context::GetCurrent()->Global(), 2, argv);
  }
  return Undefined();
}