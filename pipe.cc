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

/*
  int recvlen = 0;
  char buff[AIRTRASH_BUFFER_SIZE];
  memset(buff, '0', sizeof(buff));

  int tries = AIRTRASH_MAX_TRIES;
  while (tries > 0) {
    while ((recvlen =
                (int)read(socket.sock, buff, AIRTRASH_BUFFER_SIZE)) > 0) {
      printf("(CLIENT) bytes received %d\n", recvlen);
      fwrite(buff, sizeof *buff, recvlen, fp);
      tries = AIRTRASH_MAX_TRIES;
    }
    tries--;
  }

  if (recvlen < 0) {
    printf("(CLIENT) \033[91mERROR: read error\033[m\n");
  }
*/

// FIXME: implement recv
str pipe_fast_recv(int sock) {
  std::vector<char> buffer(AIRTRASH_BUFFER_SIZE);
  str rcv;
  int bytesReceived = 0;
  do {
    bytesReceived = (int)recv(sock, &buffer[0], buffer.size(), MSG_NOSIGNAL);
    if (bytesReceived == -1 || errno == EAGAIN) {
      continue;
    }
    rcv.append(buffer.cbegin(), buffer.cend());
  } while (bytesReceived == AIRTRASH_BUFFER_SIZE);
  return rcv;
}

bool pipe_send(int sock, unsigned char *buff, int len) {
  int pos = 0, sendlen = 0;
  int tries = AIRTRASH_MAX_TRIES;
  while (pos < len) {
    if (tries < 0)
      return false;
    sendlen = (int)send(sock, &buff[pos], len - pos, MSG_NOSIGNAL);
    if (sendlen < 0 || errno == EAGAIN) {
      tries--;
      continue;
    }
    tries = AIRTRASH_MAX_TRIES;
    pos += sendlen;
  }
  return true;
}

bool pipe_fast_send(int sock, str buff) {
  return pipe_send(sock,
                   reinterpret_cast<unsigned char *>(
                       const_cast<char *>(buff.c_str())),
                   buff.size());
}
