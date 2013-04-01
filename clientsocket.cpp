#include "clientsocket.h"
#include "sstream"

ClientSocket::ClientSocket(std::string host, int port) {
    if (!Socket::do_create()) {
        perror("socket create");
    }

    if (!Socket::do_connect(host, port)) {
        perror("socket connect");
    }
}

const ClientSocket& ClientSocket::read(std::string& s) {
    if (!Socket::do_receive(s)) {
        perror("socket read");
    }

    return *this;
}

const ClientSocket& ClientSocket::read(int &i) {
    std::string s; 

    // call other version that reads a string
    read(s);
    
    std::istringstream iss(s);
    iss >> i;

    return *this;
}

const ClientSocket& ClientSocket::write(const std::string& s) {
    if (!Socket::do_send(s)) {
        perror("write error");
    }

    return *this;
}

const ClientSocket& ClientSocket::write(const int &i) {
    std::ostringstream oss;
    oss << i;

    // call other version that takes a string
    write(oss.str());

    return *this;
}

