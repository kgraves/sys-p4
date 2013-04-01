#include "serversocket.h"

ServerSocket::ServerSocket(int port) {
    if (!Socket::do_create()) {
        perror("socket create");
    }

    if (!Socket::do_bind(port)) {
        perror("socket bind");
    }

    if (!Socket::do_listen()) {
        perror("socket listen");
    }
}

ServerSocket::~ServerSocket() {
    close(Socket::sock());
}

const ServerSocket& ServerSocket::read(std::string &s) {
    if (!Socket::do_receive(s)) {
        perror("socket receive");
    }

    return *this;
}

const ServerSocket& ServerSocket::write(const std::string &s) {
    if (!Socket::do_send(s)) {
        perror("socket send");
    }

    return *this;
}

void ServerSocket::accept(ServerSocket &sock) {
    if (!Socket::do_accept(sock)) {
        perror("socket accept");
    }
}

void ServerSocket::close_socket() {
    close(Socket::sock());
}
