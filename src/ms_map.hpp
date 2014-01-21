#ifndef _NODE_MS_MAP_H
#define _NODE_MS_MAP_H

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

#include <mapserver.h>

#include "ms_error.hpp"
#include "ms_layers.hpp"
#include "ms_outputformat.hpp"
#include "ms_rect.hpp"
#include "ms_projection.hpp"
#include "ms_hashtable.hpp"

using namespace v8;
using namespace node;

class MSMap: public ObjectWrap {
public:
  static Persistent<FunctionTemplate> constructor;
  static void Initialize(Handle<Object> target);
  static Handle<Value> New(const Arguments &args);
  static Handle<Value> New(mapObj *map);
  
  static Handle<Value> SelectOutputFormat (const Arguments& args);
  static Handle<Value> SetExtent (const Arguments& args);
  static Handle<Value> DrawMap (const Arguments& args);
  static Handle<Value> Save (const Arguments& args);
  static void DrawMapAfter(uv_work_t *req);
  static void DrawMapWork(uv_work_t *req);
  
  static Handle<Value> Recompute (const Arguments& args);
  static Handle<Value> InsertLayer (const Arguments& args);
  static Handle<Value> SetSymbolSet(const Arguments &args);
  static Handle<Value> GetLabelCache(const Arguments &args);
  
  // static Handle<Value> Copy (const Arguments& args);
  static Handle<Value> PropertyGetter (Local<String> property, const AccessorInfo& info);
  static void PropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info);
  
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
