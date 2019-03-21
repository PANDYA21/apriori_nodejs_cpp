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
      printf("arr->Get(%d): %f\n", index, arr->Get(index)->NumberValue());
      index++;
    }
  }
  return arr;
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