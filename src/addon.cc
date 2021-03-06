// mineon.cc
#include <node.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace demo {

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


std::vector<unsigned int> getFreq(std::vector<double> flattrans, std::vector<double> uniqs) {
  std::vector<unsigned int> ans;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    unsigned int thisfreq = 0;
    for (unsigned int j = 0; j < flattrans.size(); j++) {
      if (uniqs[i] == flattrans[j]) {
        thisfreq++;
      }
    }
    ans.push_back(thisfreq);
  }
  return ans;
}

unsigned int count(Local<Array> trans, double item) {
  unsigned int count = 0;
  unsigned int len = trans->Length();
  for (unsigned int i = 0; i < len; i++) {
    Local<Array> thisTran = Local<Array>::Cast(trans->Get(i));
    unsigned int thisLen = thisTran->Length();
    for (unsigned int j = 0; j < thisLen; j++) {
      double thisItem = thisTran->Get(j)->NumberValue();
      if (thisItem == item) {
        count++;
      }
    }
  }
  return count;
}


// This is the implementation of the "mine" method
// Input arguments are passed using the
// const FunctionCallbackInfo<Value>& args struct
void Mine(const FunctionCallbackInfo<Value>& args) {
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
  std::vector<unsigned int> freqs = getFreq(associatedItems, uniqs);

  // since antecedent has to be present in all transactions, nA = len
  unsigned int nA = len;

  // shape the return array
  Local<Array> returnArray = Array::New(isolate);
  unsigned int index = 0;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    if (freqs[i] != 0) {
      double consequent = uniqs[i];
      if (consequent == antecedent) {
        continue;
      }
      unsigned int nAB = freqs[i];
      double confidence = (double)nAB / (double)nA;

      Local<Object> obj = Object::New(isolate);
      obj->Set(
        String::NewFromUtf8(isolate, "antecedent"), 
        Number::New(isolate, antecedent)
      );
      obj->Set(
        String::NewFromUtf8(isolate, "consequent"), 
        Number::New(isolate, consequent)
      );
      obj->Set(
        String::NewFromUtf8(isolate, "nAB"), 
        Number::New(isolate, nAB)
      );
      obj->Set(
        String::NewFromUtf8(isolate, "nA"), 
        Number::New(isolate, nA)
      );
      obj->Set(
        String::NewFromUtf8(isolate, "confidence"), 
        Number::New(isolate, confidence)
      );

      returnArray->Set(index, obj);
      index++;
    }
  }



  // Set the return value (using the passed in FunctionCallbackInfo<Value>&)
  args.GetReturnValue().Set(returnArray);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "mine", Mine);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  
// namespace demo