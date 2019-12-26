#ifndef SOCKET_H
#define SOCKET_H

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

#include "util.h"
#include "config.h"

struct Address {
  str ip;
  int port;

  struct sockaddr_in unix_sockaddr;

  Address() { this->unix_sockaddr_init(); }
  Address(str _address) {
    this->unix_sockaddr_init();
    this->set_address(_address);
  }

  void set_address(str _address) {
    this->ip = _address.substr(0, _address.find(":"));
    this->port = std::stoi(remove_to(_address, ":", 0));
    this->unix_sockaddr_update();
  }

  void set_ip(str _ip) {
    this->ip = _ip;
    this->unix_sockaddr_update();
  }

  void set_port(int _port) {
    this->port = _port;
    this->unix_sockaddr_update();
  }

  void unix_sockaddr_init() {
    memset(&this->unix_sockaddr, 0, sizeof(this->unix_sockaddr));
    this->unix_sockaddr.sin_family = AF_INET;
  }

  void unix_sockaddr_update() {
    this->unix_sockaddr.sin_port = htons((__uint16_t)this->port);
    this->unix_sockaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
  }
};

class Socket {
  Address address;

public:
  int sock;

  Socket() {};
  Socket(Address _address);

  bool create();
  bool connect();
  bool listen();
  bool fullduplex();
  void close();
};

#endif
