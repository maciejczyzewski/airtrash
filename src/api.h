#ifndef NATIVE_EXTENSION_GRAB_H
#define NATIVE_EXTENSION_GRAB_H

#include <nan.h>

#include "config.h"

NAN_METHOD(scan);
NAN_METHOD(push);
NAN_METHOD(pull);

NAN_METHOD(get_local_ip);

#endif
