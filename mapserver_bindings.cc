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
#include <node_events.h>
#include <node_buffer.h>
#include <string.h>
#include <assert.h>

using namespace v8;
using namespace node;

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

/*
#define OPT_STR_ARG(I, VAR, DEFAULT)                                    \
  String::Utf8Value * VAR;                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsString()) {                                     \
    VAR = args[I]->ToString();                                          \
  } else {                                                              \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  }
*/

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

#define RETURN_MAP(MAP)                                               \
	if (!MAP) return Null();                                            \
	HandleScope scope;                                                  \
	Local<Value> _arg_ = External::New(MAP);                            \
	Persistent<Object> _map_(Map::constructor_template->GetFunction()->NewInstance(1, &_arg_)); \
	return scope.Close(_map_);


#define RETURN_DATA()                                                   \
  HandleScope scope;                                                    \
  Local<Value> result = Encode(data, size);                             \
  delete[] data;                                                        \
  return scope.Close(result);

class Mapserver {

  public:
    static void Init(Handle<Object> target) 
    {
      HandleScope scope;
      
      NODE_SET_METHOD(target, "loadMap", LoadMap);
      
      Map::Init(target);
    }

  protected:
    
    static Handle<Value> LoadMap (const Arguments& args) {
      REQ_STR_ARG(0, filename);
      REQ_STR_ARG(1, path);
      
      mapObj * map = msLoadMap(*filename, *path);
      
      RETURN_MAP(map);
    }
  
    class Map : public EventEmitter {
      public:
        static Persistent<FunctionTemplate> constructor_template;
      
        static void Init(v8::Handle<Object> target) {
          HandleScope scope;
        
          Local<FunctionTemplate> t = FunctionTemplate::New(New);
          constructor_template = Persistent<FunctionTemplate>::New(t);
        
          t->Inherit(EventEmitter::constructor_template);
          t->InstanceTemplate()->SetInternalFieldCount(1);
        
          NODE_SET_PROTOTYPE_METHOD(t, "drawMapRaw", DrawMapRaw);
          NODE_SET_PROTOTYPE_METHOD(t, "drawMapBuffer", DrawMapBuffer);
        
          t->PrototypeTemplate()->SetAccessor(String::NewSymbol("width"), WidthGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
          t->PrototypeTemplate()->SetAccessor(String::NewSymbol("height"), HeightGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
        
          target->Set(String::NewSymbol("Map"), t->GetFunction());
        }
      
        static Handle<Value> New(const Arguments& args) {
          HandleScope scope;
          REQ_EXT_ARG(0, map);
          (new Map((mapObj *)map->Value()))->Wrap(args.This());
          return args.This();
        }

      protected:  
        Map(mapObj * map) : _map(map) { }

        ~Map() { 
          if (_map) {
            msFreeMap(_map);
          }
        }

        mapObj * _map;

        operator mapObj* () const { return _map; }
        
        static Handle<Value> Destroy (const Arguments &args) {
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          msFreeMap(map->_map);
          return Undefined();
        }

        static Handle<Value> WidthGetter (Local<String> property, const AccessorInfo& info) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(info.This());

          Local<Number> result = Integer::New(map->_map->width);
          return scope.Close(result);
        }
  
        static Handle<Value> HeightGetter (Local<String> property, const AccessorInfo& info) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(info.This());

          Local<Number> result = Integer::New(map->_map->height);
          return scope.Close(result);
        }
  
        static Handle<Value> DrawMapBuffer (const Arguments& args) {
          HandleScope scope;
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          imageObj * im = msDrawMap(map->_map, MS_FALSE);
          int size;

          unsigned char * data = msSaveImageBuffer(im, &size, map->_map->outputformat);
          Buffer *retbuf = Buffer::New((char *) data, (size_t) size);
          
          msFree(data);
          msFreeImage(im);

          return scope.Close(retbuf->handle_);
        }
        static Handle<Value> DrawMapRaw (const Arguments& args) {
          Map *map = ObjectWrap::Unwrap<Map>(args.This());
          imageObj * im = msDrawMap(map->_map, MS_FALSE);
          int size;
          unsigned char * data = msSaveImageBuffer(im, &size, map->_map->outputformat);
          RETURN_DATA();
        }
    };

};

Persistent<FunctionTemplate> Mapserver::Map::constructor_template;

extern "C"
void init (Handle<Object> target)
{
  HandleScope scope;
  Mapserver::Init(target);
}
