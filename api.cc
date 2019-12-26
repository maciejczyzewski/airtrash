#include "api.h"
#include "sandbox.h"

void scan(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  printf("\033[92m SCAN \033[m\n");

  const int port_low = 9000;
  const int port_high = 9100;

  str local_ip = get_local_ip();
  str local_mask = get_local_mask();

  printf("local_ip=%s\n", local_ip.c_str());
  printf("local_mask=%s\n", local_mask.c_str());

  std::deque<str> vec_lan = get_ips_in_range(local_ip, local_mask);

  str OUTPUT = "";

  for (str ip : vec_lan) {
    printf("[%s]\n", ip.c_str());
    for (int port = port_low; port < port_high; port++) {
      printf(".");
      str filename = port_is_open(ip, port);
      if (strcmp(filename.c_str(), "") != 0) {
        printf("%s ------> \033[92m OPEN %d \033[m file=%s\n", ip.c_str(), port,
               filename.c_str());
        OUTPUT += "|" + ip + ":" + std::to_string(port) + "&" + filename + "|";
      }
    }
    printf("\n");
  }

  //v8::String::Utf8Value out1(isolate, OUTPUT);
  //v8::Local<v8::String> out1 = v8::String::NewFromUtf8(isolate, OUTPUT.c_str(), v8::NewStringType::kNormal);

  //args.GetReturnValue().Set(out1);
  args.GetReturnValue().Set(Nan::New<v8::String>(OUTPUT).ToLocalChecked());
}

//#include <thread>

// void push_instance(Server server) {
//    server.do2();
//}

void push(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  // Check the number of arguments passed.
  if (args.Length() < 2) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
                                v8::NewStringType::kNormal)
            .ToLocalChecked()));
    return;
  }

  // Check the argument types
  if (!args[0]->IsString() || !args[1]->IsString()) {
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8(isolate, "Wrong arguments",
                                v8::NewStringType::kNormal)
            .ToLocalChecked()));
    return;
  }

  v8::String::Utf8Value arg0(isolate, args[0]);
  v8::String::Utf8Value arg1(isolate, args[1]);

  Address address = Address(str(*arg0));
  str path = str(*arg1);

  printf("--> [%s]\n", address.ip.c_str());
  Server server = Server(address);
  server.set_path(path);
  server.do2();
  // std::thread t1(push_instance, server);

  //args.GetReturnValue().Set(5);
}

void pull(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();

  // Check the number of arguments passed.
  if (args.Length() < 2) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
                                v8::NewStringType::kNormal)
            .ToLocalChecked()));
    return;
  }

  // Check the argument types
  if (!args[0]->IsString() || !args[1]->IsString()) {
    isolate->ThrowException(v8::Exception::TypeError(
        v8::String::NewFromUtf8(isolate, "Wrong arguments",
                                v8::NewStringType::kNormal)
            .ToLocalChecked()));
    return;
  }

  v8::String::Utf8Value arg0(isolate, args[0]);
  v8::String::Utf8Value arg1(isolate, args[1]);

  Address address = Address(str(*arg0));
  str path = str(*arg1);

  // Address address = Address(args[0].As<v8::String>()->Value());
  // str path = args[1].As<v8::String>()->Value();

  printf("--> [%s]\n", address.ip.c_str());
  Client client = Client(address);
  client.set_path(path);
  client.do2();

  //args.GetReturnValue().Set(5);
}

void get_local_ip(const Nan::FunctionCallbackInfo<v8::Value> &args) {
  printf("\033[92m GET_LOCAL_IP \033[m\n");
  str local_ip = get_local_ip();
  args.GetReturnValue().Set(Nan::New<v8::String>(local_ip).ToLocalChecked());
}
