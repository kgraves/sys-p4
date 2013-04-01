#include "serversocket.h"
#include "clientsocket.h"

#include "socketutils.h"
#include "serverutils.h"
#include "threadutils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

// functions
void register_servers(vector<server_info> &servers, const int &num_servers);

int main (int argc, char **argv) {
    if (argc != 2) {
        cout << "usage: [exec name] [number of servers]" << endl;
        perror("Illegal number of arguments");
    }
    else {
        int num_servers = atoi(argv[1]);

        // can only have a max of 4 worker servers
        if (num_servers > 4) {
            perror("num_servers is too damn high!");
            return 1;
        }

        vector<server_info> servers;
        register_servers(servers, num_servers);

        ServerSocket main_skt(MASTER_PORT);
        ServerSocket skt;
        string command;
        int index=0;

        // infinite loop
        while (index < MAX_THREADS) {
            // blocking call
            main_skt.accept(skt);
            skt.read(command); // command should == OPN
            
            for (int i=0; i<num_servers; ++i) {
                skt.write( server_info_to_string(servers[i]) );
                skt.read(command); // command should == ACK
            }

            skt.write(CLO);

            ++index;
        }

        main_skt.close_socket();
        skt.close_socket();
    }

    return 0;
}

void register_servers(vector<server_info> &servers, const int &num_servers) {
    int i = 0;
    string command;
    istringstream iss;

    ServerSocket skt(REG_PORT);
    ServerSocket worker_skt;

    // calculate host, port, and load for each server
    divy_load(servers, num_servers);

    while (i < num_servers) {
        // blocking call
        skt.accept(worker_skt);

        worker_skt.read(command); // command should == REG
        worker_skt.write( server_info_to_string(servers[i]) );
        worker_skt.read(command); // command should == CLO

        ++i;
    }

    skt.close_socket();
    worker_skt.close_socket();
}

