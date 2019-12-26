#ifndef CORE_H
#define CORE_H

#include <string>
#include "socket.h"
#include "config.h"

typedef std::string str;

void core_handle(int sock, Address client_address, str path);
void core_server_hook(Address address, str path);
void core_client_hook(Address address, str path);

#endif
