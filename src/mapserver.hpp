// v8
#include <v8.h>

// node
#include <node.h>

#include <mapserver.h>
#include <assert.h>

#include "ms_error.hpp"
#include "ms_map.hpp"
#include "ms_common.hpp"

using namespace v8;
using namespace node;

namespace node_mapserver {

  static Handle<Value> getVersionInt(const Arguments &args) {
    HandleScope scope;
    int version = msGetVersionInt();

    return scope.Close(Integer::New(version));
  }

  static Handle<Value> getVersion(const Arguments &args) {
    HandleScope scope;
    char * version = msGetVersion();

    return scope.Close(String::New(version));
  }

  static Handle<Value> supportsThreads(const Arguments &args) {
    HandleScope scope;
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

    return scope.Close(Integer::New(match));
  }
  
  static Handle<Value> resetErrorList(const Arguments &args) {
    msResetErrorList();
    return Undefined();
  }
  
  static Handle<Value> getError(const Arguments &args) {
    HandleScope scope;
    errorObj *err = msGetErrorObj();

    if (err == NULL) {
      return Null();
    }
    return scope.Close(MSError::New(err));
  }
  
  static Handle<Value> loadMap (const Arguments& args) {
    HandleScope scope;
    
    REQ_STR_ARG(0, filename);
    REQ_STR_ARG(1, path);
    
    mapObj *map = msLoadMap(*filename, *path);
    if (map == NULL) {
      THROW_ERROR(Error, "msLoadMap returned NULL.");
    }
    return scope.Close(MSMap::New(map));
  }
  

}