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
  // double antecedent = args[1]->NumberValue();
  double minsupp = args[2]->NumberValue();
  double minconf = args[3]->NumberValue();
  unsigned int len = input->Length();
  std::vector<double> flattrans;

  // create flat vector of transaction items
  for (unsigned int i = 0; i < len; i++) {
    Local<Array> thisTran = Local<Array>::Cast(input->Get(i));
    unsigned int thisLen = thisTran->Length();
    for (unsigned int j = 0; j < thisLen; j++) {
      double thisItem = thisTran->Get(j)->NumberValue();
      flattrans.push_back(thisItem);
    }
  }

  // create unique item vecor from the flat vector
  std::vector<double> uniqs = flattrans;
  std::sort(uniqs.begin(), uniqs.end());
  auto last = std::unique(uniqs.begin(), uniqs.end());
  uniqs.erase(last, uniqs.end());
  std::vector<unsigned int> freqs = getFreq(flattrans, uniqs);

  // filter frequent items from uniqs
  std::vector<double> frequentUniqs;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    if (freqs[i] < minsupp) {
      continue;
    }
    frequentUniqs.push_back(uniqs[i]);
  }


  // shape the return array
  Local<Array> returnArray = Array::New(isolate);
  unsigned int index = 0;

  for (unsigned int i = 0; i < frequentUniqs.size(); i++) {
    std::vector<double> thisFlattrans;

    // create flat vector of transaction items
    for (unsigned int ii = 0; ii < len; ii++) {
      Local<Array> thisTran = Local<Array>::Cast(input->Get(ii));
      unsigned int thisLen = thisTran->Length();
      bool itemPresentInThisTran = false;
      for (unsigned int j = 0; j < thisLen; j++) {
        double thisItem = thisTran->Get(j)->NumberValue();
        if (thisItem == uniqs[i]) {
          itemPresentInThisTran = true;
        }
      }
      if (!itemPresentInThisTran) {
        continue;
      }
      for (unsigned int jj = 0; jj < thisLen; jj++) {
        double thisItem = thisTran->Get(jj)->NumberValue();
        thisFlattrans.push_back(thisItem);
      }
    }

    // create unique item vecor from the flat vector
    std::vector<double> thisUniqs = thisFlattrans;
    std::sort(thisUniqs.begin(), thisUniqs.end());
    auto last = std::unique(thisUniqs.begin(), thisUniqs.end());
    thisUniqs.erase(last, thisUniqs.end());
    std::vector<unsigned int> thisFreqs = getFreq(thisFlattrans, thisUniqs);

    double antecedent = uniqs[i];
    unsigned int nA = thisUniqs.size();
    for (unsigned int iii = 0; iii < thisUniqs.size(); iii++) {
      if (thisFreqs[iii] != 0) {
        if (thisFreqs[iii] < minsupp) {
          continue;
        }
        double consequent = thisUniqs[iii];
        if (consequent == antecedent) {
          continue;
        }
        unsigned int nAB = thisFreqs[iii];
        double confidence = (double)nAB / (double)nA;
        if (confidence < minconf) {
          continue;
        }

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