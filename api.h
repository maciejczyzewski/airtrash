#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>

// Example top-level functions. These functions demonstrate how to return
// various js types. Implementations are in functions.cc

const int PORT_LOW = 9000;
const int PORT_HIGH = 9100;

NAN_METHOD(scan);
NAN_METHOD(push);
NAN_METHOD(pull);

NAN_METHOD(get_local_ip);

#endif
