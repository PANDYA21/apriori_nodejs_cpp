// mineon.cc
#include <node.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <ctime>

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
  unsigned int siz = uniqs.size();
  unsigned int flattransiz = flattrans.size();
  std::vector<unsigned int> ans(siz);

  for (unsigned int i = 0; i < siz; i++) {
    unsigned int thisfreq = 0;
    for (unsigned int j = 0; j < flattransiz; j++) {
      if (uniqs.at(i) == flattrans.at(j)) {
        thisfreq++;
      }
    }
    ans.at(i) = thisfreq;
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
  Local<Array> uniqsarr = Local<Array>::Cast(args[2]);
  Local<Array> associatedarr = Local<Array>::Cast(args[3]);
  unsigned int len = input->Length();

  // convert the js-array to cpp-vector
  unsigned int uniqsLen = uniqsarr->Length();
  std::vector<double> uniqs(uniqsLen);
  unsigned int associatedItemsLen = associatedarr->Length();
  std::vector<double> associatedItems(associatedItemsLen);
  for (unsigned int i = 0; i < associatedItemsLen; i++) {
    if (i < uniqsLen) {
      double thisItem = uniqsarr->Get(i)->NumberValue();
      uniqs.at(i) = thisItem;
    }
    double thisItem = associatedarr->Get(i)->NumberValue();
    associatedItems.at(i) = thisItem;
  }

  // count the frequencies <- this is where it takes max time
  std::time_t result = std::time(nullptr);
  printf("%s\n", std::asctime(std::localtime(&result)));
  std::vector<unsigned int> freqs = getFreq(associatedItems, uniqs);
  std::time_t result2 = std::time(nullptr);
  printf("%s\n", std::asctime(std::localtime(&result2)));

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
  // unsigned int index = 0;
  // for (unsigned int i = 0; i < uniqs.size(); i++) {
  //   returnArray->Set(index, Number::New(isolate, uniqs[i]));
  //   index++;
  // }

  // shape the return array 2
  Local<Array> returnArray2 = Array::New(isolate);
  unsigned int index2 = 0;
  for (unsigned int i = 0; i < associatedItems.size(); i++) {
    if (i < uniqs.size()) {
      returnArray->Set(index2, Number::New(isolate, uniqs[i]));
    }
    returnArray2->Set(index2, Number::New(isolate, associatedItems[i]));
    index2++;
  }

  // shape the return object
  Local<Object> returnObject = Object::New(isolate);
  returnObject->Set(
    String::NewFromUtf8(isolate, "uniqs"), 
    returnArray
  );
  returnObject->Set(
    String::NewFromUtf8(isolate, "associatedItems"), 
    returnArray2
  );


  // Set the return value (using the passed in FunctionCallbackInfo<Value>&)
  args.GetReturnValue().Set(returnObject);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "mine", Mine);
  NODE_SET_METHOD(exports, "getConsequents", getConsequents);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  
// namespace demo