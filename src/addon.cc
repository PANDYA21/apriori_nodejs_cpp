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

// double* cast1dArray(Local<Array> input) {
//   unsigned int len = input->Length();
//   double* output = 0;
//   output = new double[len];
//   for (unsigned int i = 0; i < len; i++) {
//     output[i] = input->Get(i)->NumberValue();
//   }
//   return output;
// }

// // cast cpp array from js array (2d arrays)
// double** cast2dArray(Local<Array> input) {
//   unsigned int num = input->Length();
//   double** output = 0;
//   output = new double*[num];

//   for (unsigned int i = 0; i < num; i++) {
//     Local<Array> arr = Local<Array>::Cast(input->Get(i));
//     unsigned int lnum = arr->Length();
//     output[i] = new double[lnum];

//     for (unsigned int j = 0; j < lnum; j++) {
//       Local<Value> v = arr->Get(j);
//       double value = v->NumberValue();

//       output[i][j] = value;
//     }
//   }
//   return output;
// }

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


// std::vector<double> getUniqueItems(Local<Array> arr, Isolate* isolate) {
//   Local<Array> arr2 = Array::New(isolate);
//   unsigned int len = arr->Length();
//   for (unsigned int i = 0; i < len; i++) {
//     arr2->Set(i, arr->Get(i));
//   }
//   double* arr3 = cast1dArray(arr2);
//   std::vector<double> v(arr3, arr3 + len);

//   std::sort(v.begin(), v.end());
//   std::vector<double> v2 = v;
//   auto last = std::unique(v.begin(), v.end());
//   v.erase(last, v.end());
  
//   return v;
// }

// Local<Array> getItems(Local<Array> input, Isolate* isolate) {
//   Local<Array> arr = Array::New(isolate);
//   unsigned int index = 0;
//   unsigned int nTrans = input->Length();
//   for (unsigned int i = 0; i < nTrans; i++) {
//     Local<Array> thisArray = Local<Array>::Cast(input->Get(i));
//     unsigned int nItemsInThisTran = thisArray->Length();
//     for (unsigned int j = 0; j < nItemsInThisTran; j++) {
//       arr->Set(index, thisArray->Get(j));
//       index++;
//     }
//   }
//   return arr;
// }

// std::vector<double> js1dToVect(Local<Array> jsArr) {
//   double* cppArr = cast1dArray(jsArr);
//   unsigned int len = jsArr->Length();
//   std::vector<double> vect(cppArr, cppArr + len);
//   return vect;
// }

// Local<Array> vectToJs1d(std::vector<unsigned int> vect, Isolate* isolate) {
//   Local<Array> arr = Array::New(isolate);
//   for (unsigned int i = 0; i < vect.size(); i++) {
//     Local<Number> retval = v8::Number::New(isolate, vect[i]);
//     arr->Set(i, retval);
//   }
//   return arr;
// }

bool containsVector(std::vector<double> vect, double item) {
  return std::find(vect.begin(), vect.end(), item) != vect.end();
}

// std::vector<double> getAssociations(Local<Array> trans, double item) {
//   unsigned int len = trans->Length();
//   std::vector<double> associatedItems;

//   for (unsigned int i = 0; i < len; i++) {
//     Local<Array> thisTran = Local<Array>::Cast(trans->Get(i));
//     unsigned int thisLen = thisTran->Length();

//     std::vector<double> thisVect = js1dToVect(thisTran);
//     if (containsVector(thisVect, item)) {
//       for (unsigned int j = 0; j < thisVect.size(); ++j) {
//         if (thisVect[j] != item) {
//           associatedItems.push_back(thisVect[j]);
//         }
//       }
//     }

//   }
//   return associatedItems;
// }

// unsigned int count(Local<Array> trans, double item) {
//   unsigned int count = 0;
//   unsigned int len = trans->Length();
//   for (unsigned int i = 0; i < len; i++) {
//     Local<Array> thisTran = Local<Array>::Cast(trans->Get(i));
//     unsigned int thisLen = thisTran->Length();
//     for (unsigned int j = 0; j < thisLen; j++) {
//       double thisItem = thisTran->Get(j)->NumberValue();
//       if (thisItem == item) {
//         count++;
//       }
//     }
//   }
//   return count;
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
  unsigned int nA = len; // count(input, antecedent);

  Local<Array> returnArray = Array::New(isolate);
  unsigned int index = 0;
  for (unsigned int i = 0; i < uniqs.size(); i++) {
    if (freqs[i] != 0) {
      double consequent = uniqs[i];
      unsigned int nB = 0; // count(input, consequent);
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
        String::NewFromUtf8(isolate, "nB"), 
        Number::New(isolate, nB)
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