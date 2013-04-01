#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "socket.h"
#include <cstdio>

class ServerSocket : protected Socket {
    public:
        ServerSocket(int port);
        ServerSocket(){};
        ~ServerSocket();

        const ServerSocket& read(std::string &s);
        const ServerSocket& write(const std::string &s);

        void accept(ServerSocket&);

        void close_socket();
};

#endif
