#ifndef PIPE_H
#define PIPE_H

#include <string>
#include "config.h"

typedef std::string str;

bool pipe_fast_send(int sock, str data_to_send);
str pipe_fast_recv(int sock);
bool pipe_send(int sock, unsigned char *buff, int len, int max_tries = AIRTRASH_MAX_TRIES);

#endif
