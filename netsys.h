#ifndef NETSYS_H
#define NETSYS_H

#include <deque>
#include <string>

#include "config.h"

typedef std::string str;

str netsys_local_ip();
str netsys_local_mask();
std::deque<str> netsys_range(str ip, str mask);
str netsys_is_open(str ip, int port);

#endif
