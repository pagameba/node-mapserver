#include "ms_hashtable.hpp"
#include "ms_common.hpp"

Persistent<FunctionTemplate> MSHashTable::constructor;

void MSHashTable::Initialize(Handle<Object> target) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MSHashTable::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("Hashtable"));

  constructor->InstanceTemplate()->SetNamedPropertyHandler(NamedGetter, NamedSetter, NULL, NULL, NULL);

}

MSHashTable::MSHashTable(hashTableObj *table) : ObjectWrap(), this_(table) {}

MSHashTable::MSHashTable() : ObjectWrap(), this_(0) {}

MSHashTable::~MSHashTable() { }

Handle<Value> MSHashTable::New(const Arguments &args) {
  HandleScope scope;
  MSHashTable *obj;

  if (!args.IsConstructCall()) {
    return ThrowException(String::New("Cannot call constructor as function, you need to use 'new' keyword"));
  }

  if (args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void* ptr = ext->Value();
    obj = static_cast<MSHashTable*>(ptr);
    obj->Wrap(args.This());
    return args.This();
  }

  return args.This();
}

Handle<Value> MSHashTable::New(hashTableObj *table) {
  return ClosedPtr<MSHashTable, hashTableObj>::Closed(table);
}

Handle<Value> MSHashTable::NamedGetter (Local<String> property, const AccessorInfo& info) {
  MSHashTable *table = ObjectWrap::Unwrap<MSHashTable>(info.This());
  v8::String::AsciiValue key(property);
  char *value;
  hashTableObj *metadataTable = table->this_;

  value = msLookupHashTable(metadataTable, *key);
  if (value == NULL){
    return Undefined();
  }
  RETURN_STRING(value);
}

Handle<Value> MSHashTable::NamedSetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
  MSHashTable *table = ObjectWrap::Unwrap<MSHashTable>(info.Holder());
  v8::String::AsciiValue key(property);
  v8::String::AsciiValue val(value);

  msInsertHashTable(table->this_, *key, *val);

  RETURN_STRING(*val);
}


