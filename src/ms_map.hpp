#ifndef _NODE_MS_MAP_H
#define _NODE_MS_MAP_H

#include <nan.h>

#include <mapserver.h>

#include "ms_error.hpp"
#include "ms_layer.hpp"
#include "ms_layers.hpp"
#include "ms_outputformat.hpp"
#include "ms_rect.hpp"
#include "ms_projection.hpp"
#include "ms_hashtable.hpp"
#include "utils.hpp"

class MSMap: public Nan::ObjectWrap {
public:
  static Nan::Persistent<v8::FunctionTemplate> constructor;
  static void Initialize(v8::Local<v8::Object> target);
  static NAN_METHOD(New);
  static v8::Local<v8::Value> NewInstance(mapObj *map);

  static NAN_METHOD(Clone);
  static NAN_METHOD(SelectOutputFormat);
  static NAN_METHOD(SetExtent);
  static NAN_METHOD(DrawMap);
  static NAN_METHOD(Save);
  static void EIO_DrawMap(uv_work_t* req);
  static void EIO_AfterDrawMap(uv_work_t* req);

  static NAN_METHOD(Recompute);
  static NAN_METHOD(InsertLayer);
  static NAN_METHOD(SetSymbolSet);
  static NAN_METHOD(GetLabelCache);

  // static Handle<Value> Copy (const Arguments& args);
  static NAN_GETTER(PropertyGetter);
  static NAN_SETTER(PropertySetter);

  MSMap();
  MSMap(mapObj *map);
  inline mapObj *get() { return this_; }
  mapObj *this_;

  MSLayers *layers_;
  MSRect *extent;
  MSOutputFormat *outputformat;
  // labelCacheObj labelcache;

private:
  ~MSMap();

};

#endif
