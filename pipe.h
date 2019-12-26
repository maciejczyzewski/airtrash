#ifndef PIPE_H
#define PIPE_H

#include <string>

typedef std::string str;

// BEZPIECZNY PRZESYL DANYCH Z POTWIERDZENIEM??? I PAKIETY?

bool pipe_fast_send(int sock, str data_to_send);
str pipe_fast_recv(int sock);

#endif
