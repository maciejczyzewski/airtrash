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
#include <thread>
#include <unistd.h>
#include <vector>

#include "config.h"
#include "netsys.h"
#include "pipe.h"
#include "socket.h"
#include "util.h"

typedef std::string str;

#define HANDLE_RET close(sock); return;

void handle(int sock, Address client_address, str path) {
  printf("(SERVER) pull request from %s:%d\n", client_address.ip.c_str(),
         client_address.port);

  str data = pipe_fast_recv(sock);
  printf("(SERVER) probe cmd=`%s`\n", data.c_str());

  if (strcmp(data.c_str(), "scan") == 0) {
    pipe_fast_send(sock, str(AIRTRASH_MAGICWORD) + "|" + path);
    HANDLE_RET;
  }

  if (strcmp(data.c_str(), "pull") != 0) {
    printf("(SERVER) not implemented behavior\n");
    HANDLE_RET;
  }

  FILE *fp = fopen(path.c_str(), "rb");
  if (fp == NULL) {
      printf("(SERVER) \033[91mERROR: error opening file `%s`\033[m\n",
         path.c_str());
    HANDLE_RET;
  }

  unsigned char buff[AIRTRASH_BUFFER_SIZE];
  memset(buff, '0', sizeof(buff));

  for (;;) {
    int nread = (int)fread(buff, sizeof *buff, AIRTRASH_BUFFER_SIZE, fp);

    if (nread > 0) {
      printf("(SERVER) sending nread=%d\n", nread);
      pipe_send(sock, buff, nread);
    }

    if (nread < AIRTRASH_BUFFER_SIZE) {
      if (feof(fp)) {
        printf("(SERVER) end of file\n");
        break;
      }
      if (ferror(fp)) {
        printf("(SERVER) \033[91mERROR: error reading file\033[m\n");
        break;
      }
    }
  }

  HANDLE_RET;
}

void server_func(Address address, str path) {
  int sock;
  Address client_address = Address();
  Socket socket = Socket(netsys_first_free_slot(address));
  socket.fullduplex();

  while (true) {
    if ((sock = accept(socket.sock,
                       (struct sockaddr *)&client_address.unix_sockaddr,
                       &client_address.unix_sockaddr_len)) < 0) {
      printf("could not open a socket to accept data\n");
    }

    client_address.propagate();
    handle(sock, client_address, path);
  }

  socket.close();
}

void client_func(Address address, str path) {
  FILE *fp = fopen(path.c_str(), "wb");
  if (NULL == fp) {
    printf("(CLIENT) \033[91mERROR: error opening file `%s`\033[m\n",
           path.c_str());
    return;
  }

  Socket socket = Socket(address);
  socket.fullduplex();

  pipe_fast_send(socket.sock, "pull");

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

  fclose(fp);
  socket.close();
}
