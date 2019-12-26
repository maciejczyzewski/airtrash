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

#include "config.h"
#include "netsys.h"
#include "pipe.h"
#include "util.h"
#include "socket.h"

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

void handle(int sock, struct sockaddr_in client_address, str path) {
  printf("client connected with ip address: %s\n",
         inet_ntoa(client_address.sin_addr));

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
  Address address;
  str path;

public:
  Server(Address _address) : address(_address) {

    int shift = 0;
    // find first empty slot
    while (true) {
      if (strcmp(netsys_is_open(address.ip, address.port + shift).c_str(),
                 "") == 0) {
        break;
      } else {
        shift++;
      }
    }

    address.port += shift;

    this->socket = Socket(address);
    this->socket.fullduplex();
  }

  void set_path(str _path) { this->path = _path; }

  void do2() {
    this->action();
    this->socket.close();
  }

  void action() {
    // socket address used to store client address
    struct sockaddr_in client_address;
    socklen_t client_address_len = 0;

    // run indefinitely
    while (true) {
      // open a new socket to transmit data per connection
      int sock;
      if ((sock = accept(this->socket.sock, (struct sockaddr *)&client_address,
                         &client_address_len)) < 0) {
        printf("could not open a socket to accept data\n");
      }

      // new std::thread(std::ref(handle), client_address, this->path);
      handle(sock, client_address, this->path);
    }
  }
};

class Client {
  Socket socket;
  Address address;
  str path;

public:
  Client(Address _address) : address(_address) {
    this->socket = Socket(address);
    this->socket.fullduplex();
  }

  void set_path(str _path) { this->path = _path; }

  void do2() {
    this->action();
    this->socket.close();
  }

  void action() {
    pipe_fast_send(this->socket.sock, "pull");

    /* Create file where data will be stored */
    FILE *fp = fopen(this->path.c_str(), "wb");
    if (NULL == fp) {
      printf("CLIENT \033[94m Error opening file \033[m %s\n",
             this->path.c_str());
      // return 1;
    }

    int bytesReceived = 0;
    char buff[AIRTRASH_BUFFER_SIZE];
    memset(buff, '0', sizeof(buff));
    while ((bytesReceived =
                (int)read(this->socket.sock, buff, AIRTRASH_BUFFER_SIZE)) > 0) {
      printf("Bytes received %d\n", bytesReceived);
      fwrite(buff, sizeof *buff, bytesReceived, fp);
    }

    if (bytesReceived < 0) {
      printf("\n Read Error \n");
    }
  }
};
