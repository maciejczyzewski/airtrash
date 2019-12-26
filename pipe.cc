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

#include "pipe.h"

bool pipe_fast_send(int sock, str data_to_send) {
  int pos = 0;
  int len = (int)data_to_send.size();
  printf("1[START] pos=%d len=%d\n", pos, len);
  while (pos < len) {
    if (pos < 0) {
      return false;
    }
    printf("------> sending | pos=%d | len=%d |\n", pos, len);
    pos += (int)send(sock, data_to_send.substr(pos).c_str(), len - pos, 0);
  }
  printf("1[END]\n");
  return true;
}

str pipe_fast_recv(int sock) {
  // create the buffer with space for the data
  const unsigned int MAX_BUF_LENGTH = 4096;
  std::vector<char> buffer(MAX_BUF_LENGTH);
  str rcv;
  int bytesReceived = 0;
  do {
    bytesReceived = (int)recv(sock, &buffer[0], buffer.size(), 0);
    // append string from buffer.
    if (bytesReceived == -1) {
      // error
    } else {
      rcv.append(buffer.cbegin(), buffer.cend());
    }
  } while (bytesReceived == MAX_BUF_LENGTH);
  return rcv;
}
