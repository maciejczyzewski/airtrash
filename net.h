#ifndef NET_H
#define NET_H

#include <deque>
#include <string>

#include "socket.h"
#include "config.h"

typedef std::string str;

str net_local_ip();
str net_local_mask();
std::deque<str> net_range(str ip, str mask);
str net_is_open(str ip, int port);
Address net_first_free_slot(Address address);

#endif
