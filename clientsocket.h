#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "socket.h"
#include <cstdio>

class ClientSocket : private Socket {
    public:
        ClientSocket ( std::string host, int port );

        const ClientSocket& read(std::string &s);
        const ClientSocket& read(int &s);

        const ClientSocket& write(const std::string &s);
        const ClientSocket& write(const int &i);
};

#endif

