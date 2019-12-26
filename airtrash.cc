#include "src/api.h"

using v8::FunctionTemplate;

#define NAN_REGISTER(name) \
  Nan::Set(target, Nan::New(#name).ToLocalChecked(), \
           Nan::GetFunction(Nan::New<FunctionTemplate>(name)).ToLocalChecked());


NAN_MODULE_INIT(InitAll) {
  NAN_REGISTER(scan);
  NAN_REGISTER(pull);
  NAN_REGISTER(push);

  NAN_REGISTER(get_local_ip);
}

NODE_MODULE(airtrash, InitAll)
