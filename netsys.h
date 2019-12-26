#ifndef NETSYS_H
#define NETSYS_H

#include <deque>
#include <string>

typedef std::string str;

str get_local_ip();
str get_local_mask();
std::deque<str> get_ips_in_range(str ip, str mask);

#endif
