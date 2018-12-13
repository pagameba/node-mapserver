#include "ms_map.hpp"

Nan::Persistent<v8::FunctionTemplate> MSMap::constructor;

void MSMap::Initialize(v8::Local<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(MSMap::New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Map").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "clone", Clone);
  Nan::SetPrototypeMethod(tpl, "selectOutputFormat", SelectOutputFormat);
  Nan::SetPrototypeMethod(tpl, "setExtent", SetExtent);
  Nan::SetPrototypeMethod(tpl, "drawMap", DrawMap);
  Nan::SetPrototypeMethod(tpl, "recompute", Recompute);
  Nan::SetPrototypeMethod(tpl, "insertLayer", InsertLayer);
  Nan::SetPrototypeMethod(tpl, "setSymbolSet", SetSymbolSet);
  Nan::SetPrototypeMethod(tpl, "save", Save);
  Nan::SetPrototypeMethod(tpl, "getLabelCache", GetLabelCache);

  /* Read-Write Properties */
  RW_ATTR(tpl, "name", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "status", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "width", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "height", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "maxsize", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "units", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "resolution", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "defresolution", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "shapepath", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "mappath", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "imagetype", PropertyGetter, PropertySetter);
  RW_ATTR(tpl, "projection", PropertyGetter, PropertySetter);

  /* Read-Only Properties */
  RO_ATTR(tpl, "cellsize", PropertyGetter);
  RO_ATTR(tpl, "scaledenom", PropertyGetter);
  RO_ATTR(tpl, "mimetype", PropertyGetter);
  RO_ATTR(tpl, "outputformat", PropertyGetter);
  RO_ATTR(tpl, "metadata", PropertyGetter);
  RO_ATTR(tpl, "extent", PropertyGetter);
  RO_ATTR(tpl, "layers", PropertyGetter);

  target->Set(Nan::New("Map").ToLocalChecked(), tpl->GetFunction());
  constructor.Reset(tpl);
}

MSMap::MSMap(mapObj *map) : ObjectWrap(), this_(map) {}

MSMap::MSMap() : ObjectWrap(), this_(0) {}

MSMap::~MSMap() {
  if (this_) {
    msFreeMap(this_);
    this_ = NULL;
  }
}

NAN_METHOD(MSMap::New) {
  mapObj *map;
  MSMap *obj;

  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
    return;
  }

  if (info[0]->IsExternal()) {
    v8::Local<v8::External> ext = info[0].As<v8::External>();
    void* ptr = ext->Value();
    obj = static_cast<MSMap*>(ptr);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  }

  if (info.Length() == 2) {
    if (!ISSTR(info, 0) || !ISSTR(info, 1)) {
      Nan::ThrowTypeError("Map constructor takes one or two string arguments");
      return;
    }
    map = msLoadMap(TOSTR(info[0]), TOSTR(info[1]));
  } else if (info.Length() == 1) {
    if (!ISSTR(info, 0)) {
      Nan::ThrowTypeError("Map constructor takes one or two string arguments");
      return;
    }
    map = msLoadMap(TOSTR(info[0]), NULL);
  } else {
    map = msNewMapObj();
  }

  if (map == NULL) {
    Nan::ThrowError("Unable to load requested map.");
    return;
  }

  obj = new MSMap(map);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Value> MSMap::NewInstance(mapObj *ptr) {
  Nan::EscapableHandleScope scope;
  MSMap* obj = new MSMap();
  obj->this_ = ptr;
  v8::Local<v8::Value> ext = Nan::New<v8::External>(obj);
  return scope.Escape(Nan::New(constructor)->GetFunction()->NewInstance(1, &ext));
}

NAN_METHOD(MSMap::Clone) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  mapObj * _copy = msNewMapObj();
  if (msCopyMap(_copy, map->this_) == MS_SUCCESS) {
    info.GetReturnValue().Set(MSMap::NewInstance(_copy));
  }
}


NAN_METHOD(MSMap::SetExtent) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  if (info.Length() != 4) {
    Nan::ThrowError("SetExtent requires 4 numeric arguments");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber()) {
    Nan::ThrowError("SetExtent requires 4 numeric arguments");
    return;
  }

  map->this_->extent.minx = Nan::To<double>(info[0]).FromJust();
  map->this_->extent.miny = Nan::To<double>(info[1]).FromJust();
  map->this_->extent.maxx = Nan::To<double>(info[2]).FromJust();
  map->this_->extent.maxy = Nan::To<double>(info[3]).FromJust();

  info.GetReturnValue().Set(Nan::True());
}

NAN_METHOD(MSMap::Recompute) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  mapObj *_map = map->this_;
  _map->cellsize = msAdjustExtent(&(_map->extent),
                                   _map->width,
                                   _map->height);
  msCalculateScale(_map->extent,
                   _map->units,
                   _map->width,
                   _map->height,
                   _map->resolution,
                   &_map->scaledenom);
  info.GetReturnValue().Set(Nan::True());
}

NAN_METHOD(MSMap::InsertLayer) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  MSLayer *layer;
  v8::Local<v8::Object> obj;
  int result;
  int position = -1;

  if (info.Length() < 1) {
    Nan::ThrowError("insertLayer requires at least one argument");
    return;
  }

  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("first argument to project must be Layer object");
    return;
  }

  obj = info[0]->ToObject();

  if (obj->IsNull() || obj->IsUndefined() || !Nan::New(MSLayer::constructor)->HasInstance(obj)) {
    Nan::ThrowTypeError("first argument to project must be Layer object");
    return;
  }

  layer = Nan::ObjectWrap::Unwrap<MSLayer>(obj);

  if (info.Length() == 2) {
    if (!info[1]->IsNumber()) {
      Nan::ThrowTypeError("second argument must be an integer");
      return;
    } else {
      position = Nan::To<int>(info[1]).FromJust();
      if (position >= map->this_->numlayers) {
        position = -1;
      }
    }
  }
  result = msInsertLayer(map->this_, layer->this_, position);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(MSMap::SelectOutputFormat) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  if (info.Length() != 1 || !info[0]->IsString()) {
    Nan::ThrowTypeError("selectOuptutFormat requires a string argument");
    return;
  }
  outputFormatObj * format = msSelectOutputFormat(map->this_, TOSTR(info[0]));
  if ( format == NULL) {
    Nan::ThrowError("Output format not supported.");
    return;
  }
  msApplyOutputFormat(&(map->this_->outputformat), format, MS_NOOVERRIDE,
      MS_NOOVERRIDE, MS_NOOVERRIDE );
}

NAN_METHOD(MSMap::SetSymbolSet) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  int result;

  if (info.Length() != 1 || !info[0]->IsString()) {
    Nan::ThrowTypeError("SetSymbolSet requires a string argument");
    return;
  }

  msFreeSymbolSet(&(map->this_->symbolset));
  msInitSymbolSet(&(map->this_->symbolset));

  // Set symbolset filename
  map->this_->symbolset.filename = strdup(TOSTR(info[0]));

  // Symbolset shares same fontset as main mapfile
  map->this_->symbolset.fontset = &(map->this_->fontset);

  result = msLoadSymbolSet(&(map->this_->symbolset), map->this_);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(MSMap::Save) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());
  int result;
  if (info.Length() != 1 || !info[0]->IsString()) {
    Nan::ThrowTypeError("Save requires a string argument");
    return;
  }

  result = msSaveMap(map->this_, TOSTR(info[0]));
  info.GetReturnValue().Set(result);
}

NAN_METHOD(MSMap::GetLabelCache) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());

  labelCacheObj *labelcache = &(map->this_->labelcache);

#if MS_VERSION_NUM < 60500
  labelCacheSlotObj *cacheslot;
  int numLabels = 0;
  int k = 0;
  size_t i = 0;
  size_t j = 0;
  for(i = 0; i < MS_MAX_LABEL_PRIORITY; ++i) {
    cacheslot = &(labelcache->slots[i]);
    for(size_t j = 0; j < cacheslot->numlabels; ++j) {
      if (cacheslot->labels[j].status == MS_ON) {
        numLabels ++;
      }
    }
  }
  v8::Local<v8::Array> labels = Nan::New<v8::Array>(numLabels);
  for(i = 0; i < MS_MAX_LABEL_PRIORITY; ++i) {
    cacheslot = &(labelcache->slots[i]);
    v8::Local<v8::Array> labels = Nan::New<v8::Array>(cacheslot->numlabels);
    for(j = 0; j < cacheslot->numlabels; ++j) {
      v8::Local<v8::Object> label = Nan::New<v8::Object>();
      Nan::Set(label, Nan::New("status").ToLocalChecked(), Nan::New(MS_ON));
      Nan::Set(label, Nan::New("x").ToLocalChecked(), Nan::New(cacheslot->labels[j].point.x));
      Nan::Set(label, Nan::New("y").ToLocalChecked(), Nan::New(cacheslot->labels[j].point.y));
      Nan::Set(label, Nan::New("text").ToLocalChecked(), Nan::New(cacheslot->labels[j].labels[0].annotext).ToLocalChecked());
      Nan::Set(label, Nan::New("layerindex").ToLocalChecked(), Nan::New(cacheslot->labels[j].layerindex));
      Nan::Set(label, Nan::New("classindex").ToLocalChecked(), Nan::New(cacheslot->labels[j].classindex));
      Nan::Set(labels, k++, label);
    }
  }
#else
  v8::Local<v8::Array> labels = Nan::New<v8::Array>(labelcache->num_rendered_members);
  int p = 0;
  for (p=0; p<labelcache->num_rendered_members; p++) {
    labelCacheMemberObj* curCachePtr = labelcache->rendered_text_symbols[p];
    v8::Local<v8::Object> label = Nan::New<v8::Object>();
    Nan::Set(label, Nan::New("status").ToLocalChecked(), Nan::New(MS_ON));
    Nan::Set(label, Nan::New("x").ToLocalChecked(), Nan::New(curCachePtr->point.x));
    Nan::Set(label, Nan::New("y").ToLocalChecked(), Nan::New(curCachePtr->point.y));
    if ((curCachePtr->textsymbols[0]->annotext) == NULL) {
      Nan::Set(label, Nan::New("text").ToLocalChecked(), Nan::New("").ToLocalChecked());
    } else {
      Nan::Set(label, Nan::New("text").ToLocalChecked(), Nan::New(curCachePtr->textsymbols[0]->annotext).ToLocalChecked());
    }
    Nan::Set(label, Nan::New("layerindex").ToLocalChecked(), Nan::New(curCachePtr->layerindex));
    Nan::Set(label, Nan::New("classindex").ToLocalChecked(), Nan::New(curCachePtr->classindex));
    Nan::Set(labels, p, label);
  }
#endif

  // return an array of rendered labels
  info.GetReturnValue().Set(labels);
}

NAN_GETTER(MSMap::PropertyGetter) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());

  if (STRCMP(property, "width")) {
    info.GetReturnValue().Set(map->this_->width);
  } else if (STRCMP(property, "height")) {
    info.GetReturnValue().Set(map->this_->height);
  } else if (STRCMP(property, "status")) {
    info.GetReturnValue().Set(map->this_->status);
  } else if (STRCMP(property, "maxsize")) {
    info.GetReturnValue().Set(map->this_->maxsize);
  } else if (STRCMP(property, "cellsize")) {
    info.GetReturnValue().Set(map->this_->cellsize);
  } else if (STRCMP(property, "units")) {
    info.GetReturnValue().Set(map->this_->units);
  } else if (STRCMP(property, "scaledenom")) {
    info.GetReturnValue().Set(map->this_->scaledenom);
  } else if (STRCMP(property, "resolution")) {
    info.GetReturnValue().Set(map->this_->resolution);
  } else if (STRCMP(property, "defresolution")) {
    info.GetReturnValue().Set(map->this_->defresolution);
  } else if (STRCMP(property, "imagetype")) {
    info.GetReturnValue().Set(Nan::New(map->this_->imagetype).ToLocalChecked());
  } else if (STRCMP(property, "mimetype")) {
    info.GetReturnValue().Set(Nan::New(map->this_->outputformat->mimetype).ToLocalChecked());
  } else if (STRCMP(property, "shapepath")) {
    info.GetReturnValue().Set(Nan::New(map->this_->shapepath).ToLocalChecked());
  } else if (STRCMP(property, "mappath")) {
    info.GetReturnValue().Set(Nan::New(map->this_->mappath).ToLocalChecked());
  } else if (STRCMP(property, "name")) {
    info.GetReturnValue().Set(Nan::New(map->this_->name).ToLocalChecked());
  } else if (STRCMP(property, "outputformat")) {
    info.GetReturnValue().Set(MSOutputFormat::NewInstance(map->this_->outputformat));
  } else if (STRCMP(property, "projection")) {
    info.GetReturnValue().Set(MSProjection::NewInstance(&map->this_->projection));
  } else if (STRCMP(property, "layers")) {
    info.GetReturnValue().Set(MSLayers::NewInstance(map->this_));
  } else if (STRCMP(property, "metadata")) {
#if MS_VERSION_NUM < 60400
    info.GetReturnValue().Set(Nan::New<v8::Object>());
#else
    info.GetReturnValue().Set(MSHashTable::NewInstance(&(map->this_->web.metadata)));
#endif
  } else if (STRCMP(property, "extent")) {
    info.GetReturnValue().Set(MSRect::NewInstance(&map->this_->extent));
  }
}

NAN_SETTER(MSMap::PropertySetter) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());

  if (STRCMP(property, "width")) {
    map->this_->width = value->Int32Value();
  } else if (STRCMP(property, "height")) {
    map->this_->height = value->Int32Value();
  } else if (STRCMP(property, "maxsize")) {
    map->this_->maxsize = value->Int32Value();
  } else if (STRCMP(property, "units")) {
    int32_t units = value->Int32Value();
    if (units >= MS_INCHES && units <= MS_NAUTICALMILES) {
      map->this_->units = (MS_UNITS) units;
    }
  } else if (STRCMP(property, "resolution")) {
    map->this_->resolution = value->NumberValue();
  } else if (STRCMP(property, "defresolution")) {
    map->this_->defresolution = value->NumberValue();
  } else if (STRCMP(property, "name")) {
    REPLACE_STRING(map->this_->name, value);
  } else if (STRCMP(property, "imagetype")) {
    REPLACE_STRING(map->this_->imagetype, value);
  } else if (STRCMP(property, "shapepath")) {
    REPLACE_STRING(map->this_->shapepath, value);
  } else if (STRCMP(property, "projection")) {
    msLoadProjectionString(&(map->this_->projection), TOSTR(value));
  } else if (STRCMP(property, "mappath")) {
    REPLACE_STRING(map->this_->mappath, value);
  }
}

struct drawmap_baton {
  uv_work_t request;
  MSMap *map;
  errorObj * error;
  int size;
  char * data;
  Nan::Persistent<v8::Function> cb;
};

void FreeImageBuffer(char *data, void *hint) {
  msFree(data);
}

void MSMap::EIO_DrawMap(uv_work_t *req) {
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
}

void MSMap::EIO_AfterDrawMap(uv_work_t *req) {
  Nan::HandleScope scope;

  drawmap_baton *baton = static_cast<drawmap_baton *>(req->data);
  Nan::AsyncResource resource("mapserver:callback");
  if (baton->data != NULL) {
    v8::Local<v8::Value> buffer = Nan::NewBuffer(baton->data, baton->size, FreeImageBuffer, NULL).ToLocalChecked();
    v8::Local<v8::Value> argv[2] = { Nan::Null(), buffer };
    Nan::Callback *callback = new Nan::Callback(Nan::New(baton->cb));
    callback->Call(Nan::GetCurrentContext()->Global(), 2, argv, &resource);

  } else {
    v8::Local<v8::Value> argv[1] = { MSError::NewInstance(baton->error) };
    Nan::Callback *callback = new Nan::Callback(Nan::New(baton->cb));
    callback->Call(Nan::GetCurrentContext()->Global(), 1, argv, &resource);
  }

  baton->map->Unref();
  baton->cb.Reset();
  delete baton;
}

/**
 * callback for buffer creation to free the memory assocatiated with the
 * image after its been copied into the buffer
 */

NAN_METHOD(MSMap::DrawMap) {
  MSMap *map = Nan::ObjectWrap::Unwrap<MSMap>(info.Holder());

  if (info.Length() != 1) {
    Nan::ThrowTypeError("DrawMap missing callback function");
    return;
  }

  if (!info[0]->IsFunction()) {
    Nan::ThrowTypeError("DrawMap requires a single function argument");
    return;
  }

  char * version = msGetVersion();
  int match;
  match = 0;
  if (msEvalRegex("SUPPORTS\\=THREADS", version) == MS_TRUE) {
    match = 1;
  } else {
    // discard the error reported by msEvalRegex saying that it failed
    msResetErrorList();
  }


  if (match == 1) {
    drawmap_baton * baton = new drawmap_baton();
    baton->request.data = (void*) baton;
    baton->map = map;
    baton->cb.Reset(info[0].As<v8::Function>());

    map->Ref();
    uv_queue_work(uv_default_loop(),
      &baton->request,
      EIO_DrawMap,
      (uv_after_work_cb) EIO_AfterDrawMap);
  } else {
    v8::Local<v8::Value> argv[2];
    argv[0] = Nan::Null();
    imageObj * im = msDrawMap(map->this_, MS_FALSE);

    if (im != NULL) {
      int size;
      char * data = (char *)msSaveImageBuffer(im, &size, map->this_->outputformat);
      msFreeImage(im);
      argv[1] = Nan::NewBuffer(data, size, FreeImageBuffer, NULL).ToLocalChecked();
    } else {
      errorObj * err = msGetErrorObj();
      argv[0] = MSError::NewInstance(err);
      argv[1] = Nan::Null();
    }

    Nan::AsyncResource resource("mapserver:callback");

    Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());
    callback->Call(Nan::GetCurrentContext()->Global(), 2, argv, &resource);
  }
}