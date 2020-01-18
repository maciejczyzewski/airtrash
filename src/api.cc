#include "api.h"
#include "core.h"
#include "net.h"

#define V8_CHECK_ARGS_2                                                        \
  if (args.Length() < 2) {                                                     \
    isolate->ThrowException(v8::Exception::TypeError(                          \
        v8::String::NewFromUtf8(isolate, "Wrong number of arguments",          \
                                v8::NewStringType::kNormal)                    \
            .ToLocalChecked()));                                               \
    return;                                                                    \
  }                                                                            \
                                                                               \
  if (!args[0]->IsString() || !args[1]->IsString()) {                          \
    isolate->ThrowException(v8::Exception::TypeError(                          \
        v8::String::NewFromUtf8(isolate, "Wrong arguments",                    \
                                v8::NewStringType::kNormal)                    \
            .ToLocalChecked()));                                               \
    return;                                                                    \
  }

void get_local_ip(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  args.GetReturnValue().Set(
      Nan::New<v8::String>(net_local_ip()).ToLocalChecked());
}

void scan(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  printf("[[[\033[92mSCAN\033[m]]]\n");

  str protocol = "";
  str local_ip = net_local_ip();
  str local_mask = net_local_mask();

  printf("local_ip=%s\n", local_ip.c_str());
  printf("local_mask=%s\n\n", local_mask.c_str());

  for (str ip : net_range(local_ip, local_mask)) {
    printf("[%s]\n", ip.c_str());
    for (int port = AIRTRASH_PORT_LOW; port < AIRTRASH_PORT_HIGH; port++) {
      printf(".");
      str filename = net_is_open(ip, port);
      if (strcmp(filename.c_str(), "65") == 0) {
        break;
      }
      if (strcmp(filename.c_str(), "64") == 0) {
        break;
      }
      if (strcmp(filename.c_str(), "13") == 0) {
        break;
      }
      if (strcmp(filename.c_str(), "22") == 0) {
        break;
      }
      if (strcmp(filename.c_str(), "") != 0 and
          filename.find("/") != std::string::npos) {
        printf("\n\033[92mOPEN\033[m %s:%d file=%s\n", ip.c_str(), port,
               filename.c_str());
        protocol +=
            "|" + ip + ":" + std::to_string(port) + "&" + filename + "|";
      }
    }
    printf("\n\n");
  }

  args.GetReturnValue().Set(Nan::New<v8::String>(protocol).ToLocalChecked());
}

void push(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  V8_CHECK_ARGS_2;

  v8::String::Utf8Value arg0(isolate, args[0]);
  v8::String::Utf8Value arg1(isolate, args[1]);

  core_server_hook(Address(str(*arg0)), str(*arg1));
}

void pull(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  V8_CHECK_ARGS_2;

  v8::String::Utf8Value arg0(isolate, args[0]);
  v8::String::Utf8Value arg1(isolate, args[1]);

  core_client_hook(Address(str(*arg0)), str(*arg1));
}
