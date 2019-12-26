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

bool send_str2(int sock, unsigned char *data_to_send, int _len) {
  int pos = 0;
  int len = _len;
  printf("1[START] pos=%d len=%d\n", pos, len);
  while (pos < len) {
    if (pos < 0) {
      return false;
    }
    printf("------> sending | pos=%d | len=%d |\n", pos, len);
    pos += (int)send(sock, &data_to_send[pos], len - pos, 0);
  }
  printf("1[END]\n");
  return true;
}

void handle(int sock, Address client_address, str path) {
  printf("client connected with ip address: %s:%d\n", client_address.ip.c_str(),
         client_address.port);

  str data = pipe_fast_recv(sock);
  printf("received: '%s' (new)\n", data.c_str());
  if (strcmp(data.c_str(), "scan") == 0) {
    printf("\033[92m IT's SCAN \033[m");
    pipe_fast_send(sock, str(AIRTRASH_MAGICWORD) + "|" + path); // FIXME
    close(sock);
    return;
  }
  if (strcmp(data.c_str(), "pull") != 0) {
    printf("JAKIS PRZYPALOWIEC?\n");
    return;
  }

  /* Open the file that we wish to transfer */
  printf("---------> want to read `%s`\n", path.c_str());
  FILE *fp = fopen(path.c_str(), "rb");
  if (fp == NULL) {
    printf("File opern error");
    // return 1;
  }
  printf("OKAY, HAVE FILE?\n");
  /* Read data from file and send it */
  for (;;) {
    /* First read file in chunks of AIRTRASH_BUFFER_SIZE bytes */
    unsigned char buff[AIRTRASH_BUFFER_SIZE];
    int nread = (int)fread(buff, sizeof *buff, AIRTRASH_BUFFER_SIZE, fp);
    printf("Bytes read %d \n", nread);

    /* If read was success, send data. */
    if (nread > 0) {
      printf("Sending \n");
      printf("--> %p\n", buff);
      send_str2(sock, buff, nread);
    }

    if (nread < AIRTRASH_BUFFER_SIZE) {
      if (feof(fp))
        printf("End of file\n");
      if (ferror(fp))
        printf("Error reading\n");
      break;
    }
  }

  close(sock);
}

class Server {
  Socket socket;
  str path;

public:
  Server(Address address) {
    this->socket = Socket(netsys_first_free_slot(address));
    this->socket.fullduplex();
  }

  void set_path(str _path) { this->path = _path; }

  void do2() {
    this->action();
    this->socket.close();
  }

  void action() {
    Address client_address = Address();

    while (true) {
      int sock;

      if ((sock = accept(this->socket.sock,
                         (struct sockaddr *)&client_address.unix_sockaddr,
                         &client_address.unix_sockaddr_len)) < 0) {
        printf("could not open a socket to accept data\n");
      }

      client_address.propagate();
      handle(sock, client_address, this->path);
    }
  }
};

void client_func(Address address, str path) {
  FILE *fp = fopen(path.c_str(), "wb");
  if (NULL == fp) {
    printf("(CLIENT) \033[91mERROR: error opening file `%s`\033[m\n", path.c_str());
    return;
  }

  Socket socket = Socket(address);
  socket.fullduplex();

  pipe_fast_send(socket.sock, "pull");

  int bytesReceived = 0;
  char buff[AIRTRASH_BUFFER_SIZE];
  memset(buff, '0', sizeof(buff));
  while ((bytesReceived = (int)read(socket.sock, buff, AIRTRASH_BUFFER_SIZE)) >
         0) {
    printf("(CLIENT) bytes received %d\n", bytesReceived);
    fwrite(buff, sizeof *buff, bytesReceived, fp);
  }

  if (bytesReceived < 0) {
    printf("(CLIENT) \033[91mERROR: read error\033[m\n");
  }

  socket.close();
}
