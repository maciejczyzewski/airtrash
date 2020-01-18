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

#include "net.h"
#include "pipe.h"
#include "socket.h"
#include "util.h"

str net_local_ip() {
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
         str(ifa->ifa_name).find("eth") != std::string::npos ||
         str(ifa->ifa_name).find("br") != std::string::npos)) {
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

str net_local_mask() {
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;

  getifaddrs(&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr->sa_family == AF_INET &&
        (str(ifa->ifa_name).find("en") != std::string::npos ||
         str(ifa->ifa_name).find("eth") != std::string::npos || 
         str(ifa->ifa_name).find("br") != std::string::npos)) {
      sa = (struct sockaddr_in *)ifa->ifa_netmask;
      addr = inet_ntoa(sa->sin_addr);
      return str(addr);
    }
  }

  freeifaddrs(ifap);
  return str("");
}

std::deque<str> net_range(str ip, str mask) {
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

str net_is_open(str ip, int port) {
  
  struct sockaddr_in address; /* the libc network address data structure */
  short int sock = -1;        /* file descriptor for the network socket */
  fd_set fdset;
  struct timeval tv;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(ip.c_str()); /* assign the address */
  address.sin_port = htons(port);

  /* translate int2port num */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // fcntl(sock, F_SETFL, O_NONBLOCK);

  str status = "";
  if (sock == -1)
    return status;

  long arg;
  if ((arg = fcntl(sock, F_GETFL, NULL)) < 0) {
    fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
    close(sock);
    return status;
  }
  arg |= O_NONBLOCK;
  if (fcntl(sock, F_SETFL, arg) < 0) {
    fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
    close(sock);
    return status;
  }

  // printf("BEF\n");
  int res = connect(sock, (struct sockaddr *)&address, sizeof(address));
  // printf("AFT\n");
  //
  //struct timeval tv;
  fd_set myset;
  socklen_t lon;
  int valopt;
  if (res < 0) {
    if (errno == EINPROGRESS) {
      fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
      do {
        tv.tv_sec = 0; 
        tv.tv_usec = AIRTRASH_TIMEOUT_USEC*30;
        //tv.tv_sec = 1;
        //tv.tv_usec = 0;
        FD_ZERO(&myset);
        FD_SET(sock, &myset);
        res = select(sock + 1, NULL, &myset, NULL, &tv);
        if (res < 0 && errno != EINTR) {
          fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
          close(sock);
          return std::to_string(errno);
        } else if (res > 0) {
          // Socket selected for write
          lon = sizeof(int);
          if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)(&valopt), &lon) <
              0) {
            fprintf(stderr, "Error in getsockopt() %d - %s\n", errno,
                    strerror(errno));
            close(sock);
            return status;
          }
          // Check the value returned...
          if (valopt) {
            fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt,
                    strerror(valopt));
            close(sock);
            return status;
          }
          break;
        } else {
          fprintf(stderr, "Timeout in select() - Cancelling!\n");
          close(sock);
          return std::to_string(22);
        }
      } while (1);
    } else {
      fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
      close(sock);
      return std::to_string(errno);
    }
  }
  // Set to blocking mode again...
  if ((arg = fcntl(sock, F_GETFL, NULL)) < 0) {
    fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
    close(sock);
    return status;
  }
  arg &= (~O_NONBLOCK);
  if (fcntl(sock, F_SETFL, arg) < 0) {
    fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
    close(sock);
    return status;
  }

  FD_ZERO(&fdset);
  FD_SET(sock, &fdset);
  tv.tv_sec = 0; /* timeout */
  tv.tv_usec = AIRTRASH_TIMEOUT_USEC*100;

  if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
    int so_error;
    socklen_t len = sizeof so_error;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
    // fcntl(sock, F_SETFL, O_EXLOCK);

    if (so_error == 0) {
      printf("\n\033[90mchecking peer [%d]\033[m\n", port);
      pipe_fast_send(sock, "scan");
      str data = pipe_fast_recv(sock);
      if (data.find(AIRTRASH_MAGICWORD) != std::string::npos)
        status = remove_to(data, AIRTRASH_MAGICWORD);
    }
  }

  close(sock);
  return status;
}

Address net_first_free_slot(Address address) {
  while (true) {
    if (strcmp(net_is_open(address.ip, address.port).c_str(), "") == 0)
      break;
    address.set_port(address.port + 1);
  }
  return address;
}
