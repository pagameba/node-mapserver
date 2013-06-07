/*
Copyright (c) 2010, Paul Spencer <pagameba@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <mapserver.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <string.h>
#include <assert.h>

using namespace v8;
using namespace node;

#define RW_PROPERTY(TEMPL, SYM, GETTER, SETTER) \
  TEMPL->InstanceTemplate()->SetAccessor(String::NewSymbol(SYM), GETTER, SETTER);

#define RO_PROPERTY(TEMPL, SYM, GETTER) \
  TEMPL->InstanceTemplate()->SetAccessor(String::NewSymbol(SYM), GETTER, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);


#define THROW_ERROR(TYPE, STR)                                          \
  return ThrowException(Exception::TYPE(String::New(STR)));

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return ThrowException(Exception::TypeError(                         \
                             String::New("Expected " #N "arguments"))); 

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsInt32())                      \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an integer")));  \
  VAR = args[I]->Int32Value();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (args.Length() <= (I) || !args[I]->IsNumber())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a number")));    \
  VAR = args[I]->NumberValue();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsExternal())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " invalid")));             \
  Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_FUN_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a function")));  \
  Local<Function> VAR = Local<Function>::Cast(args[I]);

#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsInt32()) {                                      \
    VAR = args[I]->Int32Value();                                        \
  } else {                                                              \
    return ThrowException(Exception::TypeError(                         \
              String::New("Argument " #I " must be an integer"))); \
  }

#define RETURN_LAYER(LAYER)                                               \
  if (!LAYER) return Null();                                            \
  HandleScope scope;                                                  \
  Local<Value> _arg_ = External::New(LAYER);                            \
  Persistent<Object> _layer_(Layer::constructor_template->GetFunction()->NewInstance(1, &_arg_)); \
  return scope.Close(_layer_);

#define RETURN_MAP(MAP)                                               \
  if (!MAP) return Null();                                            \
  HandleScope scope;                                                  \
  Local<Value> _arg_ = External::New(MAP);                            \
  Persistent<Object> _map_(Map::constructor_template->GetFunction()->NewInstance(1, &_arg_)); \
  return scope.Close(_map_);

#define RETURN_ERROR(ERROR)                                               \
  if (!ERROR) return Null();                                            \
  HandleScope scope;                                                  \
  Local<Value> _arg_ = External::New(ERROR);                            \
  Persistent<Object> _err_(ErrorObj::constructor_template->GetFunction()->NewInstance(1, &_arg_)); \
  return scope.Close(_err_);

#define RETURN_STRING(STR)                                              \
  HandleScope scope;                                                    \
  Local<String> result = String::New(STR);                              \
  return scope.Close(result);

#define RETURN_NUMBER(NUM)                                                   \
  HandleScope scope;                                                    \
  Local<Number> result = Number::New(NUM);                             \
  return scope.Close(result);
  
#define REPLACE_STRING(TARGET, VALUE)                                   \
  if (VALUE->IsString()) {                                              \
    v8::String::AsciiValue _v_(VALUE->ToString());                      \
    msFree(TARGET);                                                     \
    TARGET = strdup(*_v_);                                              \
  }

class Mapserver {

  public:
    
    static int supportsThreads;
    
    static void Init(Handle<Object> target) 
    {
      HandleScope scope;
      
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_CENTER);
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_LEFT);
      NODE_DEFINE_CONSTANT(target, MS_ALIGN_RIGHT);
      NODE_DEFINE_CONSTANT(target, MS_AUTO);
      NODE_DEFINE_CONSTANT(target, MS_BITMAP);
      NODE_DEFINE_CONSTANT(target, MS_BLUE);
      NODE_DEFINE_CONSTANT(target, MS_CC);
      NODE_DEFINE_CONSTANT(target, MS_CJC_BEVEL);
      NODE_DEFINE_CONSTANT(target, MS_CJC_BUTT);
      NODE_DEFINE_CONSTANT(target, MS_CJC_MITER);
      NODE_DEFINE_CONSTANT(target, MS_CJC_NONE);
      NODE_DEFINE_CONSTANT(target, MS_CJC_ROUND);
      NODE_DEFINE_CONSTANT(target, MS_CJC_SQUARE);
      NODE_DEFINE_CONSTANT(target, MS_CJC_TRIANGLE);
      NODE_DEFINE_CONSTANT(target, MS_CL);
      NODE_DEFINE_CONSTANT(target, MS_CR);
      NODE_DEFINE_CONSTANT(target, MS_DB_CSV);
      NODE_DEFINE_CONSTANT(target, MS_DB_MYSQL);
      NODE_DEFINE_CONSTANT(target, MS_DB_ORACLE);
      NODE_DEFINE_CONSTANT(target, MS_DB_POSTGRES);
      NODE_DEFINE_CONSTANT(target, MS_DB_XBASE);
      NODE_DEFINE_CONSTANT(target, MS_DD);
      NODE_DEFINE_CONSTANT(target, MS_DEFAULT);
      NODE_DEFINE_CONSTANT(target, MS_DELETE);
      NODE_DEFINE_CONSTANT(target, MS_DONE);
      NODE_DEFINE_CONSTANT(target, MS_EMBED);
      NODE_DEFINE_CONSTANT(target, MS_FAILURE);
      NODE_DEFINE_CONSTANT(target, MS_FALSE);
      NODE_DEFINE_CONSTANT(target, MS_FEET);
      NODE_DEFINE_CONSTANT(target, MS_FILE);
      NODE_DEFINE_CONSTANT(target, MS_FILE_MAP);
      NODE_DEFINE_CONSTANT(target, MS_FILE_SYMBOL);
      NODE_DEFINE_CONSTANT(target, MS_FOLLOW);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_BEYOND);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_CONTAINS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_CROSSES);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_DISJOINT);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_DWITHIN);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_EQUALS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_INTERSECTS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_OVERLAPS);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_TOUCHES);
      NODE_DEFINE_CONSTANT(target, MS_GEOS_WITHIN);
      NODE_DEFINE_CONSTANT(target, MS_GIANT);
      NODE_DEFINE_CONSTANT(target, MS_GRATICULE);
      NODE_DEFINE_CONSTANT(target, MS_GREEN);
      NODE_DEFINE_CONSTANT(target, MS_HILITE);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_BYTE);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_FLOAT32);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_INT16);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_NULL);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_PC256);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_RGB);
      NODE_DEFINE_CONSTANT(target, MS_IMAGEMODE_RGBA);
      NODE_DEFINE_CONSTANT(target, MS_INCHES);
      NODE_DEFINE_CONSTANT(target, MS_INLINE);
      NODE_DEFINE_CONSTANT(target, MS_JOIN_ONE_TO_MANY);
      NODE_DEFINE_CONSTANT(target, MS_JOIN_ONE_TO_ONE);
      NODE_DEFINE_CONSTANT(target, MS_KILOMETERS);
      NODE_DEFINE_CONSTANT(target, MS_LARGE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_ANNOTATION);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_CHART);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_CIRCLE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_LINE);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_POINT);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_POLYGON);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_QUERY);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_RASTER);
      NODE_DEFINE_CONSTANT(target, MS_LAYER_TILEINDEX);
      NODE_DEFINE_CONSTANT(target, MS_LC);
      NODE_DEFINE_CONSTANT(target, MS_LL);
      NODE_DEFINE_CONSTANT(target, MS_LR);
      NODE_DEFINE_CONSTANT(target, MS_MAXCOLORS);
      NODE_DEFINE_CONSTANT(target, MS_MEDIUM);
      NODE_DEFINE_CONSTANT(target, MS_METERS);
      NODE_DEFINE_CONSTANT(target, MS_MILES);
      NODE_DEFINE_CONSTANT(target, MS_MULTIPLE);
      NODE_DEFINE_CONSTANT(target, MS_NAUTICALMILES);
      NODE_DEFINE_CONSTANT(target, MS_NO);
      NODE_DEFINE_CONSTANT(target, MS_NORMAL);
      NODE_DEFINE_CONSTANT(target, MS_OFF);
      NODE_DEFINE_CONSTANT(target, MS_OGR);
      NODE_DEFINE_CONSTANT(target, MS_ON);
      NODE_DEFINE_CONSTANT(target, MS_ORACLESPATIAL);
      NODE_DEFINE_CONSTANT(target, MS_PERCENTAGES);
      NODE_DEFINE_CONSTANT(target, MS_PIXELS);
      NODE_DEFINE_CONSTANT(target, MS_PLUGIN);
      NODE_DEFINE_CONSTANT(target, MS_POSTGIS);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_ATTRIBUTE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_INDEX);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_POINT);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_RECT);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_BY_SHAPE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_IS_NULL);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_MULTIPLE);
      NODE_DEFINE_CONSTANT(target, MS_QUERY_SINGLE);
      NODE_DEFINE_CONSTANT(target, MS_RASTER);
      NODE_DEFINE_CONSTANT(target, MS_RED);
      NODE_DEFINE_CONSTANT(target, MS_SDE);
      NODE_DEFINE_CONSTANT(target, MS_SELECTED);
      NODE_DEFINE_CONSTANT(target, MS_SHAPEFILE);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_LINE);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_NULL);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_POINT);
      NODE_DEFINE_CONSTANT(target, MS_SHAPE_POLYGON);
      NODE_DEFINE_CONSTANT(target, MS_SINGLE);
      NODE_DEFINE_CONSTANT(target, MS_SMALL);
      NODE_DEFINE_CONSTANT(target, MS_SUCCESS);
      NODE_DEFINE_CONSTANT(target, MS_TILED_SHAPEFILE);
      NODE_DEFINE_CONSTANT(target, MS_TINY );
      NODE_DEFINE_CONSTANT(target, MS_TRUE);
      NODE_DEFINE_CONSTANT(target, MS_TRUETYPE);
      NODE_DEFINE_CONSTANT(target, MS_UC);
      NODE_DEFINE_CONSTANT(target, MS_UL);
      NODE_DEFINE_CONSTANT(target, MS_UNKNOWN);
      NODE_DEFINE_CONSTANT(target, MS_UNUSED_1);
      NODE_DEFINE_CONSTANT(target, MS_UR);
      NODE_DEFINE_CONSTANT(target, MS_URL);
      NODE_DEFINE_CONSTANT(target, MS_WFS);
      NODE_DEFINE_CONSTANT(target, MS_WMS);
      NODE_DEFINE_CONSTANT(target, MS_XY);
      NODE_DEFINE_CONSTANT(target, MS_YES);
      
      NODE_SET_METHOD(target, "loadMap", LoadMap);
      NODE_SET_METHOD(target, "loadMapFromString", LoadMapFromString);
      NODE_SET_METHOD(target, "getVersion", GetVersion);
      NODE_SET_METHOD(target, "getVersionInt", GetVersionInt);
      NODE_SET_METHOD(target, "supportsThreads", SupportsThreads);

      NODE_SET_METHOD(target, "resetErrorList", ResetErrorList);
      NODE_SET_METHOD(target, "getError", GetError);
      
      ErrorObj::Init(target);
      Map::Init(target);
      Layer::Init(target);
      
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
      Mapserver::supportsThreads = match;
    }

  protected:
    
    static Handle<Value> LoadMap (const Arguments& args) {
      REQ_STR_ARG(0, filename);
      REQ_STR_ARG(1, path);
      
      mapObj * map = msLoadMap(*filename, *path);
      if (map == NULL) {
        THROW_ERROR(Error, "Ugh");
      }
      RETURN_MAP(map);
    }

    static Handle<Value> LoadMapFromString (const Arguments& args) {
      REQ_STR_ARG(0, buffer);
      REQ_STR_ARG(1, path);
      mapObj * map = msLoadMapFromString(*buffer, *path);
      if (map == NULL) {
        THROW_ERROR(Error, "Ugh");
      }
      RETURN_MAP(map);
    }
    
    static Handle<Value> ResetErrorList (const Arguments& args) {
      msResetErrorList();
      return Undefined();
    }
    
    static Handle<Value> GetError (const Arguments& args) {
      errorObj * err = msGetErrorObj();
      
      if (err == NULL) {
        return Undefined();
      } 
      RETURN_ERROR(err);
    }
    
    static Handle<Value> GetVersion (const Arguments& args) {
      HandleScope scope;
      char * version = msGetVersion();

      Local<String> result = String::New(version);
      return scope.Close(result);
    }
    
    static Handle<Value> GetVersionInt (const Arguments& args) {
      HandleScope scope;
      int version = msGetVersionInt();

      Local<Number> result = Integer::New(version);
      return scope.Close(result);
    }

    static Handle<Value> SupportsThreads (const Arguments& args) {
      HandleScope scope;
      Local<Number> result = Integer::New(Mapserver::supportsThreads);
      
      return scope.Close(result);
    }
    
    
    class ErrorObj : public ObjectWrap {
    public:
      static Persistent<FunctionTemplate> constructor_template;
      
      static void Init(v8::Handle<Object> target) {
        HandleScope scope;
        
        Local<FunctionTemplate> t = FunctionTemplate::New(New);
        constructor_template = Persistent<FunctionTemplate>::New(t);
        
        t->InstanceTemplate()->SetInternalFieldCount(1);
        // t->InstanceTemplate()->SetNamedPropertyHandler(NamedPropertyGetter, NULL, NamedPropertyQuery, NULL, NULL);
        t->InstanceTemplate()->SetNamedPropertyHandler(NamedPropertyGetter, NULL, NamedPropertyQuery, NULL, NamedPropertyEnumerator);
      
        target->Set(String::NewSymbol("ErrorObj"), t->GetFunction());
      }
      
      static Handle<Value> New(const Arguments& args) {
        HandleScope scope;
        REQ_EXT_ARG(0, err);
        (new ErrorObj((errorObj *)err->Value()))->Wrap(args.This());
        return args.This();
      }
    protected:
      ErrorObj(errorObj *err) : _err(err) { }
      
      ~ErrorObj() { }
      
      errorObj * _err;
      
      operator errorObj* () const { return _err; }
      
      static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info) {
        ErrorObj *err = ObjectWrap::Unwrap<ErrorObj>(info.This());
        v8::String::AsciiValue n(property);
        if (strcmp(*n, "code") == 0) {
          RETURN_NUMBER(err->_err->code);
        } else if (strcmp(*n, "codeStr") == 0) {
          RETURN_STRING(msGetErrorCodeString(err->_err->code));
        } else if (strcmp(*n, "message") == 0) {
          RETURN_STRING(err->_err->message);
        } else if (strcmp(*n, "routine") == 0) {
          RETURN_STRING(err->_err->routine);
        }
        return Undefined();
      }
      
      static Handle<Integer> NamedPropertyQuery(Local<String> property,
                                         const AccessorInfo& info) {
        HandleScope scope;
        ErrorObj *err = ObjectWrap::Unwrap<ErrorObj>(info.This());
        v8::String::AsciiValue n(property);
        if (strcmp(*n, "code") == 0) {
          return scope.Close(Integer::New(None));
        } else if (strcmp(*n, "codeStr") == 0) {
          return scope.Close(Integer::New(None));
        } else if (strcmp(*n, "message") == 0) {
          return scope.Close(Integer::New(None));
        } else if (strcmp(*n, "routine") == 0) {
          return scope.Close(Integer::New(None));
        }
        return Handle<Integer>();
      }
      
      static Handle<Array> NamedPropertyEnumerator(const AccessorInfo& info) {
        HandleScope scope;

        Local<Array> env = Array::New(4);
        env->Set(0, String::New("code"));
        env->Set(1, String::New("codeStr"));
        env->Set(2, String::New("message"));
        env->Set(3, String::New("routine"));

        return scope.Close(env);
      }
      
      
    };
  
  
    class Map : public ObjectWrap {
      public:
        static Persistent<FunctionTemplate> constructor_template;
      
        static void Init(v8::Handle<Object> target) {
          HandleScope scope;
        
          Local<FunctionTemplate> t = FunctionTemplate::New(New);
          constructor_template = Persistent<FunctionTemplate>::New(t);
        
          t->InstanceTemplate()->SetInternalFieldCount(1);
          
          /* Read-Write Properties */
          RW_PROPERTY(t, "name", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "status", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "width", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "height", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "maxsize", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "units", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "resolution", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "defresolution", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "shapepath", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "mappath", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "imagetype", NamedPropertyGetter, NamedPropertySetter);

          /* Read-Only Properties */
          RO_PROPERTY(t, "cellsize", NamedPropertyGetter);
          RO_PROPERTY(t, "scaledenom", NamedPropertyGetter);
          RO_PROPERTY(t, "mimetype", NamedPropertyGetter);
          
          /* psuedo object properties */
          RO_PROPERTY(t, "layers", NamedPropertyGetter);
          RO_PROPERTY(t, "extent", NamedPropertyGetter);
          
          /* Methods */
          NODE_SET_PROTOTYPE_METHOD(t, "copy", Copy);
          NODE_SET_PROTOTYPE_METHOD(t, "recompute", Recompute);
          NODE_SET_PROTOTYPE_METHOD(t, "drawMap", DrawMap);
          NODE_SET_PROTOTYPE_METHOD(t, "setExtent", SetExtent);
          NODE_SET_PROTOTYPE_METHOD(t, "selectOutputFormat", SelectOutputFormat);
        
          target->Set(String::NewSymbol("Map"), t->GetFunction());
        }
      
        static Handle<Value> New(const Arguments& args) {
          assert(args.IsConstructCall());
          HandleScope scope;
          REQ_EXT_ARG(0, map);
          (new Map((mapObj *)map->Value()))->Wrap(args.This());
          return args.This();
        }
        
      protected:  
        Map(mapObj * map) : _map(map) { 
        }

        ~Map() { 
          if (_map) {
            msFreeMap(_map);
          }
        }

        mapObj * _map;
        
        operator mapObj* () const { return _map; }
        
        static Persistent<ObjectTemplate> layers_template_;
        static Persistent<ObjectTemplate> extent_template_;
        
        static Handle<Value> Destroy (const Arguments &args) {
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          msFreeMap(map->_map);
          return Undefined();
        }

        static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info) {
          Map *map = ObjectWrap::Unwrap<Map>(info.This());
          v8::String::AsciiValue n(property);
          if (strcmp(*n, "width") == 0) {
            RETURN_NUMBER(map->_map->width);
          } else if (strcmp(*n, "height") == 0) {
            RETURN_NUMBER(map->_map->height);
          } else if (strcmp(*n, "status") == 0) {
            RETURN_NUMBER(map->_map->status);
          } else if (strcmp(*n, "maxsize") == 0) {
            RETURN_NUMBER(map->_map->maxsize);
          } else if (strcmp(*n, "cellsize") == 0) {
            RETURN_NUMBER(map->_map->cellsize);
          } else if (strcmp(*n, "units") == 0) {
            RETURN_NUMBER(map->_map->units);
          } else if (strcmp(*n, "scaledenom") == 0) {
            RETURN_NUMBER(map->_map->scaledenom);
          } else if (strcmp(*n, "resolution") == 0) {
            RETURN_NUMBER(map->_map->resolution);
          } else if (strcmp(*n, "defresolution") == 0) {
            RETURN_NUMBER(map->_map->defresolution);
          } else if (strcmp(*n, "imagetype") == 0) {
            RETURN_STRING(map->_map->imagetype);
          } else if (strcmp(*n, "mimetype") == 0) {
            RETURN_STRING(map->_map->outputformat->mimetype);
          } else if (strcmp(*n, "shapepath") == 0) {
            RETURN_STRING(map->_map->shapepath);
          } else if (strcmp(*n, "mappath") == 0) {
            RETURN_STRING(map->_map->mappath);
          } else if (strcmp(*n, "name") == 0) {
            RETURN_STRING(map->_map->name);
          } else if (strcmp(*n, "layers") == 0) {
            if (layers_template_.IsEmpty()) {
              Handle<ObjectTemplate> raw_template = ObjectTemplate::New();
              raw_template->SetInternalFieldCount(1);
              raw_template->SetIndexedPropertyHandler(LayersIndexedGetter, NULL, NULL, NULL, NULL);
              raw_template->SetNamedPropertyHandler(LayersNamedGetter, NULL, NULL, NULL, NULL);
              layers_template_ = Persistent<ObjectTemplate>::New(raw_template);
            }
            Handle<ObjectTemplate> templ = layers_template_;
            Handle<Object> result = templ->NewInstance();
            Handle<External> map_ptr = External::New(map);
            result->SetInternalField(0,map_ptr);
            HandleScope scope;
            return scope.Close(result);
          } else if (strcmp(*n, "extent") == 0) {
            if (extent_template_.IsEmpty()) {
              Handle<ObjectTemplate> raw_template = ObjectTemplate::New();
              raw_template->SetInternalFieldCount(1);
              raw_template->SetNamedPropertyHandler(ExtentNamedGetter, NULL, NULL, NULL, NULL);
              extent_template_ = Persistent<ObjectTemplate>::New(raw_template);
            }
            Handle<ObjectTemplate> templ = extent_template_;
            Handle<Object> result = templ->NewInstance();
            Handle<External> map_ptr = External::New(map);
            result->SetInternalField(0,map_ptr);
            HandleScope scope;
            return scope.Close(result);
          }
          return Undefined();
        }
        
        static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
          Map *map = ObjectWrap::Unwrap<Map>(info.Holder());
          v8::String::AsciiValue n(property);
          if (strcmp(*n, "width") == 0) {
            map->_map->width = value->Int32Value();
          } else if (strcmp(*n, "height") == 0) {
            map->_map->height = value->Int32Value();
          } else if (strcmp(*n, "maxsize") == 0) {
            map->_map->maxsize = value->Int32Value();
          } else if (strcmp(*n, "units") == 0) {
            int32_t units = value->Int32Value();
            if (units >= MS_INCHES && units <= MS_NAUTICALMILES) {
              map->_map->units = (MS_UNITS) units;
            }
          } else if (strcmp(*n, "resolution") == 0) {
            map->_map->resolution = value->NumberValue();
          } else if (strcmp(*n, "defresolution") == 0) {
            map->_map->defresolution = value->NumberValue();
          } else if (strcmp(*n, "name") == 0) {
            REPLACE_STRING(map->_map->name, value);
          } else if (strcmp(*n, "imagetype") == 0) {
            REPLACE_STRING(map->_map->imagetype, value);
          } else if (strcmp(*n, "shapepath") == 0) {
            REPLACE_STRING(map->_map->shapepath, value);
          } else if (strcmp(*n, "mappath") == 0) {
            REPLACE_STRING(map->_map->mappath, value);
          }
        }
                
        static Handle<Value> LayersIndexedGetter (uint32_t index, const AccessorInfo& info) {
          Map *map = ObjectWrap::Unwrap<Map>(info.This());
          if (index >=0 && index < map->_map->numlayers) {
            RETURN_LAYER(GET_LAYER(map->_map, index));
          }
          return Undefined();
        }
        
        static Handle<Value> LayersNamedGetter(Local<String> name, const AccessorInfo& info) {
          Map *map = ObjectWrap::Unwrap<Map>(info.This());
          String::AsciiValue n(name);
          if (strcmp(*n, "length") == 0) {
            return Integer::New(map->_map->numlayers);
          } else {
            int i;
            for (i=0; i<map->_map->numlayers; i++) {
              if (strcmp(*n, GET_LAYER(map->_map, i)->name) == 0) {
                RETURN_LAYER(GET_LAYER(map->_map, i));
              }
            }
          }
          return Undefined();
        }
        
        static Handle<Value> ExtentNamedGetter(Local<String> name, const AccessorInfo& info) {
          Map *map = ObjectWrap::Unwrap<Map>(info.This());
          String::AsciiValue n(name);
          if (strcmp(*n, "minx") == 0) {
            return Integer::New(map->_map->extent.minx);
          } else if (strcmp(*n, "miny") == 0) {
            return Integer::New(map->_map->extent.miny);
          } else if (strcmp(*n, "maxx") == 0) {
            return Integer::New(map->_map->extent.maxx);
          } else if (strcmp(*n, "maxy") == 0) {
            return Integer::New(map->_map->extent.maxy);
          }
          return Undefined();
        }
        
        static Handle<Value> Copy (const Arguments& args) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          mapObj * _map = map->_map;
          mapObj * _copy = msNewMapObj();
          if (msCopyMap(_copy, _map) == MS_SUCCESS) {
            RETURN_MAP(_copy);
          }
          return Undefined();
          
        }

        static Handle<Value> Recompute (const Arguments& args) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          mapObj * _map = map->_map;
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
        
        static void DrawMapWork(uv_work_t *req) {
          drawmap_request *drawmap_req = (drawmap_request*)req->data;
          
          imageObj * im = msDrawMap(drawmap_req->map->_map, MS_FALSE);
          if (im != NULL) {
            drawmap_req->error = NULL;
            drawmap_req->data = (char *)msSaveImageBuffer(im, &drawmap_req->size, drawmap_req->map->_map->outputformat);
            msFreeImage(im);
          } else {
            drawmap_req->error = msGetErrorObj();
            drawmap_req->data = NULL;
          }
          return;
        }
        
        static void DrawMapAfter(uv_work_t *req) {
          HandleScope scope;
          drawmap_request *drawmap_req =(drawmap_request *)req->data;
          drawmap_req->map->Unref();

          TryCatch try_catch;

          Local<Value> argv[2];
          if (drawmap_req->data != NULL) {
            Buffer * buffer = Buffer::New(drawmap_req->data, drawmap_req->size, FreeImageBuffer, NULL);

            argv[0] = Local<Value>::New(Null());
            argv[1] = Local<Value>::New(buffer->handle_);
          } else {
            Local<Value> _arg_ = External::New(drawmap_req->error);

            argv[0] = Local<Value>::New(ErrorObj::constructor_template->GetFunction()->NewInstance(1, &_arg_));
            argv[1] = Local<Value>::New(Null());
          }


          drawmap_req->cb->Call(Context::GetCurrent()->Global(), 2, argv);

          if (try_catch.HasCaught()) {
            FatalException(try_catch);
          }

          drawmap_req->cb.Dispose();
          delete drawmap_req;
          return;
        }
  
        /**
         * callback for buffer creation to free the memory assocatiated with the
         * image after its been copied into the buffer
         */
        static void FreeImageBuffer(char *data, void *hint) {
          msFree(data);
        }
        
        struct drawmap_request {
      		uv_work_t request;
        	Map *map;
          errorObj * error;
          int size;
          char * data;
          Persistent<Function> cb;
        };
  
        static Handle<Value> DrawMap (const Arguments& args) {
          HandleScope scope;
          
          REQ_FUN_ARG(0, cb);
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          
          if (Mapserver::supportsThreads) {
            drawmap_request * req = new drawmap_request();
            req->map = map;
            req->cb = Persistent<Function>::New(cb);
          
            map->Ref();
            uv_queue_work(uv_default_loop(),
              &req->request,
              DrawMapWork,
              (uv_after_work_cb) DrawMapAfter);

          } else {
            Local<Value> argv[2];
            argv[0] = Local<Value>::New(Null());
            imageObj * im = msDrawMap(map->_map, MS_FALSE);
            if (im != NULL) {
              int size;
              char * data = (char *)msSaveImageBuffer(im, &size, map->_map->outputformat);
              msFreeImage(im);
              Buffer * buffer = Buffer::New(data, size, FreeImageBuffer, NULL);
              
              argv[1] = Local<Value>::New(buffer->handle_);
            } else {
              errorObj * err = msGetErrorObj();
              Local<Value> _arg_ = External::New(err);

              argv[0] = Local<Value>::New(ErrorObj::constructor_template->GetFunction()->NewInstance(1, &_arg_));
              argv[1] = Local<Value>::New(Null());
            }
            cb->Call(Context::GetCurrent()->Global(), 2, argv);
          }
          return Undefined();
        }

        static Handle<Value> SetExtent (const Arguments& args) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          REQ_DOUBLE_ARG(0, minx);
          REQ_DOUBLE_ARG(1, miny);
          REQ_DOUBLE_ARG(2, maxx);
          REQ_DOUBLE_ARG(3, maxy);
          map->_map->extent.minx = minx;
          map->_map->extent.miny = miny;
          map->_map->extent.maxx = maxx;
          map->_map->extent.maxy = maxy;
          return scope.Close(Boolean::New(true));
        }

        static Handle<Value> SelectOutputFormat (const Arguments& args) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          REQ_STR_ARG(0, imagetype);
          outputFormatObj * format = msSelectOutputFormat(map->_map, *imagetype);
          if ( format == NULL) {
            THROW_ERROR(Error, "Output format not supported.");
          }
          msApplyOutputFormat(&(map->_map->outputformat), format, MS_NOOVERRIDE, 
              MS_NOOVERRIDE, MS_NOOVERRIDE );
          return Undefined();
        }
    };
    
    
    class Layer : public ObjectWrap {
      public:
        static Persistent<FunctionTemplate> constructor_template;
      
        static void Init(v8::Handle<Object> target) {
          HandleScope scope;
        
          Local<FunctionTemplate> t = FunctionTemplate::New(New);
          constructor_template = Persistent<FunctionTemplate>::New(t);
        
          t->InstanceTemplate()->SetInternalFieldCount(1);
        
          RW_PROPERTY(t, "name", NamedPropertyGetter, NamedPropertySetter);
          RW_PROPERTY(t, "status", NamedPropertyGetter, NamedPropertySetter);
          
          target->Set(String::NewSymbol("Layer"), t->GetFunction());
        }
      
        static Handle<Value> New(const Arguments& args) {
          HandleScope scope;
          REQ_EXT_ARG(0, layer);
          (new Layer((layerObj *)layer->Value()))->Wrap(args.This());
          return args.This();
        }

      protected:  
        Layer(layerObj * layer) : _layer(layer) { }

        ~Layer() { 
          if (_layer) {
            // msFreeLayer(_layer);
          }
        }

        layerObj * _layer;

        operator layerObj* () const { return _layer; }
        
        static Handle<Value> Destroy (const Arguments &args) {
          Layer *layer = ObjectWrap::Unwrap<Layer>(args.This());
          // msFreeLayer(layer->_layer);
          return Undefined();
        }
        
        static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info) {
          Layer *layer = ObjectWrap::Unwrap<Layer>(info.This());
          v8::String::AsciiValue n(property);
          if (strcmp(*n, "name") == 0) {
            RETURN_STRING(layer->_layer->name);
          } else if (strcmp(*n, "status") == 0) {
            RETURN_NUMBER(layer->_layer->status);
          }
          return Undefined();
        }
        
        static void NamedPropertySetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
          Layer *layer = ObjectWrap::Unwrap<Layer>(info.This());
          v8::String::AsciiValue n(property);
          if (strcmp(*n, "name") == 0) {
            v8::String::AsciiValue v(value->ToString());
            char * prev = layer->_layer->name;
            layer->_layer->name = strdup(*v);
            msFree(prev);
          } else if (strcmp(*n, "status") == 0) {
            int32_t status = value->Int32Value();
            if (status >= MS_OFF && status <= MS_DELETE) {
              layer->_layer->status = status;
            }
          }
        }
      };
};

Persistent<FunctionTemplate> Mapserver::ErrorObj::constructor_template;
Persistent<FunctionTemplate> Mapserver::Map::constructor_template;
Persistent<ObjectTemplate>   Mapserver::Map::layers_template_;
Persistent<ObjectTemplate>   Mapserver::Map::extent_template_;
Persistent<FunctionTemplate> Mapserver::Layer::constructor_template;
int Mapserver::supportsThreads;

extern "C"
void init (Handle<Object> target)
{
  HandleScope scope;
  Mapserver::Init(target);
}
