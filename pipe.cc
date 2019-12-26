#include <arpa/inet.h>
#include <errno.h>
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

#include "pipe.h"

#define MSG_NOSIGNAL 0x4000

str pipe_fast_recv(int sock) {
  str rcv;
  int recvlen = 0;
  char buff[AIRTRASH_BUFFER_SIZE];
  memset(buff, '0', sizeof(buff));
  do {
    recvlen = (int)recv(sock, &buff[0], AIRTRASH_BUFFER_SIZE, MSG_NOSIGNAL);
    if (recvlen == -1 || errno == EAGAIN) {
      continue;
    }
    rcv.append(buff, 0, recvlen);
  } while (recvlen == AIRTRASH_BUFFER_SIZE);
  return rcv;
}

bool pipe_send(int sock, unsigned char *buff, int len, int max_tries) {
  int pos = 0, sendlen = 0;
  int tries = max_tries;
  while (pos < len) {
    if (tries < 0)
      return false;
    sendlen = (int)send(sock, &buff[pos], len - pos, MSG_NOSIGNAL);
    if (sendlen < 0 || errno == EAGAIN) {
      tries--;
      continue;
    }
    tries = max_tries;
    pos += sendlen;
  }
  return true;
}

bool pipe_fast_send(int sock, str buff) {
  return pipe_send(
      sock, reinterpret_cast<unsigned char *>(const_cast<char *>(buff.c_str())),
      buff.size(), 0);
}
