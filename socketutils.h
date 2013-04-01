#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H

#include <sys/types.h>
#include <sys/socket.h>

#include <sstream>
#include <vector>

//#include "request.h"

const int CHUNK_SIZE = 100;
const int MAX_REQUESTS = 100;

const std::string OPN = "OPN";
const std::string REG = "REG";
const std::string RTS = "RTS";
const std::string ACK = "ACK";
const std::string NEG = "NEG";
const std::string CLO = "CLO";

struct socket_package {
    int command;
    int data; // port or # groups to be sent
};

#endif

