#include <arpa/inet.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "netsys.h"

str get_local_ip() {
  struct ifaddrs *ifAddrStruct = NULL;
  struct ifaddrs *ifa = NULL;
  void *tmpAddrPtr = NULL;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) {
      continue;
    }
    if (ifa->ifa_addr->sa_family == AF_INET &&
        (str(ifa->ifa_name).find("en") != std::string::npos ||
         str(ifa->ifa_name).find("eth") != std::string::npos)) {
      tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      return str(addressBuffer);
    }
  }
  return str("");

  if (ifAddrStruct != NULL)
    freeifaddrs(ifAddrStruct);
}

str get_local_mask() {
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;

  getifaddrs(&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr->sa_family == AF_INET &&
        (str(ifa->ifa_name).find("en") != std::string::npos ||
         str(ifa->ifa_name).find("eth") != std::string::npos)) {
      sa = (struct sockaddr_in *)ifa->ifa_netmask;
      addr = inet_ntoa(sa->sin_addr);
      return str(addr);
    }
  }
  return str("");
  freeifaddrs(ifap);
}

std::deque<str> get_ips_in_range(str ip, str mask) {
  struct in_addr ipaddress, subnetmask;

  inet_pton(AF_INET, ip.c_str(), &ipaddress);
  inet_pton(AF_INET, mask.c_str(), &subnetmask);

  unsigned long first_ip = ntohl(ipaddress.s_addr & subnetmask.s_addr);
  unsigned long last_ip = ntohl(ipaddress.s_addr | ~(subnetmask.s_addr));

  std::deque<str> vec_ips;
  for (unsigned long tmpip = first_ip; tmpip <= last_ip; ++tmpip) {
    struct in_addr x = {htonl(tmpip)};
    vec_ips.push_back(str(inet_ntoa(x)));
  }

  return vec_ips;
}
