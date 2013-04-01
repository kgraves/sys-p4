#ifndef SERVERUTILS_H
#define SERVERUTILS_H

#include <vector>
#include <string>
#include <sstream>
#include "hashutils.h"

const std::string LOCALHOST = "localhost";
const int MASTER_PORT = 424242;
const int REG_PORT = 323232;

struct server_info {
    std::string host;
    int port;
    int start; // inclusive
    int end; // inclusive
};

std::string server_info_to_string(server_info &si) {
    std::ostringstream oss;

    oss << si.host << " "
        << si.port << " "
        << si.start << " "
        << si.end << " ";

    return oss.str();
}

void string_to_server_info(std::string &str, server_info &si) {
    std::istringstream iss(str);

    iss >> si.host >> si.port
        >> si.start >> si.end;
}

/*
void divy_load(std::vector<server_info> &servers, const int &n) {
    int parts = (TABLE_MAX_SIZE / n) + (TABLE_MAX_SIZE % n > 0);
    server_info si;
    const int mask = 10101;

    for (int s=-1, e=parts-1, i=1; e != TABLE_MAX_SIZE-1 && s != -1; ++i) {
        // set host and port
        si.host = LOCALHOST;
        si.port = MASTER_PORT + (mask * i);

        // calculate portion
        if (s == -1)
            s = 0;
        else {
            s += parts;
            e = std::min(e+parts, TABLE_MAX_SIZE-1);
        }

        si.start = s;
        si.end = e;

        servers.push_back(si);
    }
}
*/

void divy_load(std::vector<server_info> &servers, const int &n) {
    int parts = (TABLE_MAX_SIZE / n) + (TABLE_MAX_SIZE % n > 0);
    server_info si;
    const int mask = 10101;

    if (TABLE_MAX_SIZE % n == 0) {
        // evenly distribute
        int bucket_size = TABLE_MAX_SIZE / n;

        //for (int s=-1, e=parts-1, i=1; e != TABLE_MAX_SIZE-1 && s != -1; ++i) {
        for (int i=1, s=0, e=bucket_size-1; i <= n; ++i) {
            // set host and port
            si.host = LOCALHOST;
            si.port = MASTER_PORT + (mask * i);

            si.start = s;
            si.end = e;

            s = e;
            e += bucket_size;

            servers.push_back(si);
        }
    }
    else {
        for (int s=-1, e=parts-1, i=1; e != TABLE_MAX_SIZE-1 && s != -1; ++i) {
            // set host and port
            si.host = LOCALHOST;
            si.port = MASTER_PORT + (mask * i);

            // calculate portion
            if (s == -1)
                s = 0;
            else {
                s += parts;
                e = std::min(e+parts, TABLE_MAX_SIZE-1);
            }

            si.start = s;
            si.end = e;

            servers.push_back(si);
        }
    }
}

#endif

