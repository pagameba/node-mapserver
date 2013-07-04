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
      
      RO_PROPERTY(t, "connectiontype", NamedPropertyGetter);
      
      NODE_SET_PROTOTYPE_METHOD(t, "getGridIntersectionCoordinates", GetGridIntersectionCoordinates);
      
      target->Set(String::NewSymbol("Layer"), t->GetFunction());
    }
  
    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;
      REQ_EXT_ARG(0, layer);
      (new Layer((layerObj *)layer->Value()))->Wrap(args.This());
      return args.This();
    }

    layerObj * _layer;

    operator layerObj* () const { return _layer; }
  protected:  
    Layer(layerObj * layer) : _layer(layer) { }

    ~Layer() { 
      if (_layer) {
        // msFreeLayer(_layer);
      }
    }

    
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
      } else if (strcmp(*n, "connectiontype") == 0) {
        RETURN_NUMBER(layer->_layer->connectiontype);
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
  
  static Handle<Value> GetGridIntersectionCoordinates (const Arguments& args) {
    HandleScope scope;
    Layer *layer = ObjectWrap::Unwrap<Layer>(args.This());
    
    int i = 0;
    
    graticuleIntersectionObj *values = msGraticuleLayerGetIntersectionPoints(layer->_layer->map, layer->_layer);
    
    Handle<ObjectTemplate> objTempl = ObjectTemplate::New();
    objTempl->SetInternalFieldCount(1);

    Handle<Array> left = Array::New(values->nLeft);
    Handle<Array> top = Array::New(values->nTop);
    Handle<Array> right = Array::New(values->nRight);
    Handle<Array> bottom = Array::New(values->nBottom);
    
    for (i=0; i<values->nLeft; i++) {
      Local<Object> val = objTempl->NewInstance();
      val->Set(String::New("x"), Number::New(values->pasLeft[i].x));
      val->Set(String::New("y"), Number::New(values->pasLeft[i].y));
      val->Set(String::New("label"), String::New(values->papszLeftLabels[i]));
      left->Set(i, val);
    }
    for (i=0; i<values->nTop; i++) {
      Local<Object> val = objTempl->NewInstance();
      val->Set(String::New("x"), Number::New(values->pasTop[i].x));
      val->Set(String::New("y"), Number::New(values->pasTop[i].y));
      val->Set(String::New("label"), String::New(values->papszTopLabels[i]));
      top->Set(i, val);
    }
    for (i=0; i<values->nRight; i++) {
      Local<Object> val = objTempl->NewInstance();
      val->Set(String::New("x"), Number::New(values->pasRight[i].x));
      val->Set(String::New("y"), Number::New(values->pasRight[i].y));
      val->Set(String::New("label"), String::New(values->papszRightLabels[i]));
      right->Set(i, val);
    }
    for (i=0; i<values->nBottom; i++) {
      Local<Object> val = objTempl->NewInstance();
      val->Set(String::New("x"), Number::New(values->pasBottom[i].x));
      val->Set(String::New("y"), Number::New(values->pasBottom[i].y));
      val->Set(String::New("label"), String::New(values->papszBottomLabels[i]));
      bottom->Set(i, val); 
    }
    
    // return object like this:
    // {
    //   left: [{position: 0, label: '123.00'}],
    //   top: [{position: 0, label: '123.00'}],
    //   right: [{position: 0, label: '123.00'}],
    //   bottom: [{position: 0, label: '123.00'}],
    // }
    Local<Object> result = objTempl->NewInstance();
    result->Set(String::New("left"), left);
    result->Set(String::New("top"), top);
    result->Set(String::New("right"), right);
    result->Set(String::New("bottom"), bottom);
    return scope.Close(result);
  }
};