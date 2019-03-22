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

Local<Array> getUniqueItems(Local<Array> arr, Isolate* isolate) {
  Local<Array> arr2 = Array::New(isolate);
  unsigned int len = arr->Length();
  for (unsigned int i = 0; i < len; i++) {
    arr2->Set(i, arr->Get(i));
  }
  double* arr3 = cast1dArray(arr2);
  std::vector<double> v(arr3, arr3 + len);

  std::sort(v.begin(), v.end());
  auto last = std::unique(v.begin(), v.end());
  v.erase(last, v.end()); 

  Local<Array> arr4 = Array::New(isolate);
  for (unsigned int i = 0; i < v.size(); i++) {
    printf("v[i]: %f\n", v[i]);
    // v8::Local<v8::Value> jsElement = New<v8::Value>(v[i]);
    Local<Number> retval = v8::Number::New(isolate, v[i]);
    arr4->Set(i, retval);
  }

  return arr4;
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
  return getUniqueItems(arr, isolate);
}

bool isNaN(double x) {
  return x != x;
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
  // double** output = cast2dArray(input);
  // unsigned int nTrans = input->Length();
  Local<Array> items = getItems(input, isolate);

  // for (unsigned int i = 0; i < nTrans; ++i) {
  //   Local<Array> thisArray = Local<Array>::Cast(input->Get(i));
  //   unsigned int nItemsInThisTran = thisArray->Length();
  //   for (unsigned int j = 0; j < nItemsInThisTran; ++j) {
  //     // printf("%f\n", output[i][j]);
  //     printf("%f\n", thisArray->Get(j)->NumberValue());
  //   }
  //   printf("\n");
  // }

  // Set the return value (using the passed in
  // FunctionCallbackInfo<Value>&)
  args.GetReturnValue().Set(items);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "mine", Mine);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // namespace demo