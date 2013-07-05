#ifndef __NODE_MS_ERROR_H__
#define __NODE_MS_ERROR_H__

#include <v8.h>

#include <node.h>
#include <node_object_wrap.h>

#include <mapserver.h>

using namespace v8;
using namespace node;

class MSError: public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor;
    static void Initialize(Handle<Object> target);
    static Handle<Value> New(const Arguments &args);
    static Handle<Value> New(errorObj *err);

    MSError();
    MSError(errorObj *err);
    inline errorObj *get() { return this_; }
    errorObj *this_;
    
  // protected:
  //   static Handle<Value> NamedPropertyGetter (Local<String> property, const AccessorInfo& info);
  //   static Handle<Integer> NamedPropertyQuery(Local<String> property, const AccessorInfo& info);
  //   static Handle<Array> NamedPropertyEnumerator(const AccessorInfo& info);
    
  private:
    ~MSError();
};

#endif
