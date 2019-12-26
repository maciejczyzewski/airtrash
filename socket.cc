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

#include "socket.h"

Socket::Socket(Address _address) {
  this->address = _address;
  this->create();
}

bool Socket::create() {
  if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\033[91mERROR: could not create socket\033[m\n");
    return false;
  }

  int on = 1;
  setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

  return true;
}

bool Socket::connect() {
  if (::connect(this->sock, (struct sockaddr *)&this->address.unix_sockaddr,
              sizeof(this->address.unix_sockaddr)) < 0) {
    printf("\033[91mERROR: could not connect to host\033[m\n");
    return false;
  }
  return true;
}

bool Socket::listen() {
  if ((::bind(this->sock, (struct sockaddr *)&this->address.unix_sockaddr,
            sizeof(this->address.unix_sockaddr))) < 0) {
    printf("\033[91mERROR: could not bind socket\033[m\n");
    return false;
  }

  if (::listen(this->sock, AIRTRASH_WAIT_SIZE) < 0) {
    printf("\033[91mERROR: could not open socket for listening\033[m\n");
    return false;
  }

  return true;
}

bool Socket::fullduplex() {
  bool status = false;
  status += this->listen();
  status += this->connect();
  return status;
}

void Socket::close() { ::close(this->sock); }
