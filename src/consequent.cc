// mineon.cc
#include <node.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace demo2 {

using namespace v8;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;


// This is the implementation of the "getConsequents" method
// Input arguments are passed using the
// const FunctionCallbackInfo<Value>& args struct
void getConsequents(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  // Check the argument types
  if (!args[0]->IsArray()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate,
                            "Wrong arguments",
                            NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  Local<Array> input = Local<Array>::Cast(args[0]);
  double antecedent = args[1]->NumberValue();
  unsigned int len = input->Length();
  std::vector<double> associatedItems;

  // create flat vector of associatied items
  for (unsigned int i = 0; i < len; i++) {
    Local<Array> thisTran = Local<Array>::Cast(input->Get(i));
    unsigned int thisLen = thisTran->Length();
    for (unsigned int j = 0; j < thisLen; j++) {
      double thisItem = thisTran->Get(j)->NumberValue();
      associatedItems.push_back(thisItem);
    }
  }

  // create unique item vecor from the associated vector
  std::vector<double> uniqs = associatedItems;
  std::sort(uniqs.begin(), uniqs.end());
  auto last = std::unique(uniqs.begin(), uniqs.end());
  uniqs.erase(last, uniqs.end());

  // shape the return array
  Local<Array> returnArray = Array::New(isolate);
  unsigned int index = 0;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    returnArray->Set(index, Number::New(isolate, uniqs[i]));
    index++;
  }


  // Set the return value (using the passed in FunctionCallbackInfo<Value>&)
  args.GetReturnValue().Set(returnArray);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getConsequents", getConsequents);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  
// namespace demo2