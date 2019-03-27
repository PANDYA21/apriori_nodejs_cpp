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

double* cast1dArray(Local<Array> input) {
  unsigned int len = input->Length();
  double* output = 0;
  output = new double[len];
  for (unsigned int i = 0; i < len; i++) {
    output[i] = input->Get(i)->NumberValue();
  }
  return output;
}

// cast cpp array from js array (2d arrays)
double** cast2dArray(Local<Array> input) {
  unsigned int num = input->Length();
  // double* output[num] = {};
  double** output = 0;
  output = new double*[num];

  for (unsigned int i = 0; i < num; i++) {
    Local<Array> arr = Local<Array>::Cast(input->Get(i));
    unsigned int lnum = arr->Length();
    // double output[i][lnum] = {};
    output[i] = new double[lnum];

    for (unsigned int j = 0; j < lnum; j++) {
      Local<Value> v = arr->Get(j);
      double value = v->NumberValue();

      output[i][j] = value;
      // printf("%f\n", output[i][j]);
    }
    // printf("\n");
  }
  return output;
}

std::vector<unsigned int> getFreq(std::vector<double> flattrans, std::vector<double> uniqs) {
  std::vector<unsigned int> ans;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    unsigned int thisfreq = 0;
    for (unsigned int j = 0; j < flattrans.size(); j++) {
      if (uniqs[i] == flattrans[j]) {
        thisfreq++;
      }
    }
    // printf("uniqs[%d]: %f, thisfreq: %d\n",i, uniqs[i], thisfreq);
    ans.push_back(thisfreq);
  }
  return ans;
}


// Local<Array> getUniqueItems(Local<Array> arr, Isolate* isolate) {
std::vector<double> getUniqueItems(Local<Array> arr, Isolate* isolate) {
  Local<Array> arr2 = Array::New(isolate);
  unsigned int len = arr->Length();
  for (unsigned int i = 0; i < len; i++) {
    arr2->Set(i, arr->Get(i));
  }
  double* arr3 = cast1dArray(arr2);
  std::vector<double> v(arr3, arr3 + len);

  std::sort(v.begin(), v.end());
  std::vector<double> v2 = v;
  auto last = std::unique(v.begin(), v.end());
  v.erase(last, v.end());
  // std::vector<double> freqs = getFreq(v2, v);

  return v;

  // Local<Array> arr4 = Array::New(isolate);
  // for (unsigned int i = 0; i < v.size(); i++) {
  //   printf("v[i]: %f\n", v[i]);
  //   Local<Number> retval = v8::Number::New(isolate, v[i]);
  //   arr4->Set(i, retval);
  // }

  // return arr4;
}

Local<Array> getItems(Local<Array> input, Isolate* isolate) {
  Local<Array> arr = Array::New(isolate);
  unsigned int index = 0;
  unsigned int nTrans = input->Length();
  for (unsigned int i = 0; i < nTrans; i++) {
    Local<Array> thisArray = Local<Array>::Cast(input->Get(i));
    // printf("i is: %d\n", i);
    unsigned int nItemsInThisTran = thisArray->Length();
    for (unsigned int j = 0; j < nItemsInThisTran; j++) {
      // printf("j is: %d\n", j);
      arr->Set(index, thisArray->Get(j));
      // printf("arr->Get(%d): %f\n", index, arr->Get(index)->NumberValue());
      index++;
    }
  }
  // return getUniqueItems(arr, isolate);
  return arr;
}

std::vector<double> js1dToVect(Local<Array> jsArr) {
  double* cppArr = cast1dArray(jsArr);
  unsigned int len = jsArr->Length();
  std::vector<double> vect(cppArr, cppArr + len);
  return vect;
}

Local<Array> vectToJs1d(std::vector<unsigned int> vect, Isolate* isolate) {
  Local<Array> arr = Array::New(isolate);
  for (unsigned int i = 0; i < vect.size(); i++) {
    Local<Number> retval = v8::Number::New(isolate, vect[i]);
    arr->Set(i, retval);
  }
  return arr;
}

bool containsVector(std::vector<double> vect, double item) {
  return std::find(vect.begin(), vect.end(), item) != vect.end();
}

std::vector<double> getAssociations(Local<Array> trans, double item) {
  unsigned int len = trans->Length();
  std::vector<double> associatedItems;

  for (unsigned int i = 0; i < len; i++) {
    Local<Array> thisTran = Local<Array>::Cast(trans->Get(i));
    unsigned int thisLen = thisTran->Length();

    std::vector<double> thisVect = js1dToVect(thisTran);
    if (containsVector(thisVect, item)) {
      // printf("\n%f is present in transaction id: %d\n", item, i);
      // printf("items in this trans: \n");

      // std::vector<double> filtered;
      // std::copy_if (thisVect.begin(), thisVect.end(), std::back_inserter(filtered), [](double i){ return i != item; } );
      for (unsigned int j = 0; j < thisVect.size(); ++j) {
        if (thisVect[j] != item) {
          // printf("%f\n", thisVect[j]);
          associatedItems.push_back(thisVect[j]);
        }
      }
    }

  }
  return associatedItems;
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

// std::vector<double> filterOutFromVect(std::vector<double> vect, double item) {
//   std::vector<double> filtered;
//   std::copy_if (vect.begin(), vect.end(), std::back_inserter(filtered), [](double i){ return i != item; } );
//   return filtered;
// }

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
  Local<Array> items = getItems(input, isolate);
  std::vector<double> uniqs = getUniqueItems(items, isolate);
  double antecedent = args[1]->NumberValue();

  std::vector<double> assocs = getAssociations(input, antecedent);
  std::vector<unsigned int> freqs = getFreq(assocs, uniqs);
  unsigned int nA = count(input, antecedent);

  Local<Array> returnArray = Array::New(isolate);
  unsigned int index = 0;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    if (freqs[i] != 0) {
      double consequent = uniqs[i];
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
        Number::New(isolate, freqs[i])
      );
      obj->Set(
        String::NewFromUtf8(isolate, "nA"), 
        Number::New(isolate, nA)
      );
      obj->Set(
        String::NewFromUtf8(isolate, "nB"), 
        Number::New(isolate, count(input, consequent))
      );
      returnArray->Set(index, obj);
      index++;
    }
  }



  // Set the return value (using the passed in
  // FunctionCallbackInfo<Value>&)
  // args.GetReturnValue().Set(items);
  args.GetReturnValue().Set(returnArray);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "mine", Mine);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // namespace demo