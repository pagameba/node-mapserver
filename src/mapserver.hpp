#ifndef NODE_MAPSERVER_H
#define NODE_MAPSERVER_H

#include <nan.h>
#include <mapserver.h>
#include <assert.h>

#include "utils.hpp"

#include "ms_error.hpp"
#include "ms_layer.hpp"
#include "ms_layers.hpp"
#include "ms_map.hpp"
#include "ms_outputformat.hpp"
#include "ms_point.hpp"
#include "ms_projection.hpp"
#include "ms_rect.hpp"
#include "ms_hashtable.hpp"
// #include "ms_common.hpp"

using namespace v8;
using namespace node;

namespace node_mapserver {

  static NAN_METHOD(getVersionInt) {
    Nan::HandleScope scope;
    int version = msGetVersionInt();
    info.GetReturnValue().Set(version);
  }

  static NAN_METHOD(getVersion) {
    Nan::HandleScope scope;
    char * version = msGetVersion();
    info.GetReturnValue().Set(Nan::New(version).ToLocalChecked());
  }

  static NAN_METHOD(supportsThreads) {
    Nan::HandleScope scope;
    char * version = msGetVersion();
    char const * regex = "SUPPORTS\\=THREADS";
    int match;
    match = 0;
    if (msEvalRegex(regex, version) == MS_TRUE) {
      match = 1;
    } else {
      // discard the error reported by msEvalRegex saying that it failed
      msResetErrorList();
    }
    info.GetReturnValue().Set(match);
  }

  static NAN_METHOD(resetErrorList) {
    msResetErrorList();
  }

  static NAN_METHOD(getError) {
    Nan::HandleScope scope;
    errorObj *err = msGetErrorObj();
    info.GetReturnValue().Set(MSError::NewInstance(err));
  }
}

#endif