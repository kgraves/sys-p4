#include "clientsocket.h"
#include "parser.h"
#include "request.h"

#include "socketutils.h"
#include "serverutils.h"
#include "hashutils.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Constants
const string OUT_PATH = "out.txt";

// Functions
void init_child(vector<server_info> &servers);
vector<Request> child_work(const string &in_path);
void sort_requests(vector<Request> &requests, vector<server_info> &servers, vector< vector<Request> > &buckets);
void print_buckets(vector< vector<Request> > &buckets);
void print_servers(vector<server_info> &servers);

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "usage: [exec name] [input_file]" << endl;
        perror("Illegal number of arguments");
        return 1;
    }
    else {
        // get info from main server
        vector<server_info> servers;
        init_child(servers);

        if (servers.size() == 0) {
            perror("no servers online");
            return 1;
        }

        // parse input file into requests
        vector<Request> requests = child_work(argv[1]);

        // sort each request into appropriate bucket
        vector< vector<Request> > buckets(servers.size());
        sort_requests(requests, servers, buckets);

        string reply;
        unsigned int isize = buckets.size(), jsize;
        for (unsigned int i=0; i < isize; ++i) {
            ClientSocket skt(servers[i].host, servers[i].port);

            skt.write(RTS);
            skt.read(reply); // reply should == ACK

            jsize = buckets[i].size();
            for (unsigned int j=0; j < jsize; ++j) {
                skt.write( buckets[i][j].to_string() );
                skt.read(reply); // reply should == ACK
            }

            skt.write(CLO);
        }
    }

    return 0;
}

void init_child(vector<server_info> &servers) {
    // initial communication with main server.
    //      asks for all of the servers' info and 
    //      accumulates it in the vector passed
    //      to this function
    //      also creates all ClientSockets needed
    //      for communication with servers

    string response;
    string h;
    int p, s, e;
    ClientSocket skt(LOCALHOST, MASTER_PORT);
    server_info si;
    istringstream iss;

    // initial message
    skt.write(OPN);
    skt.read(response);

    while (response != CLO) {
        iss.str(response);
        iss >> h >> p >> s >> e;

        si.host = h;
        si.port = p;
        si.start = s;
        si.end = e;

        servers.push_back(si);

        skt.write(ACK);
        skt.read(response);
    }
}

vector<Request> child_work(const string &in_path) {
    Parser p1(in_path);
    p1.mmap_parse();
    return p1.requests();
}

void sort_requests(vector<Request> &requests, vector<server_info> &servers, vector< vector<Request> > &buckets) {
    // sorts all of the requests in the argument requests
    //      into their respective bucket based on their 
    //      hash value.
    //      Assume buckets is already initialized to the 
    //      correct amount of elements

    int key;
    int server_index;

    for (vector<Request>::iterator iter=requests.begin(); iter != requests.end(); ++iter) {
        // hash key
        if (iter->m_type == STRING)
            key = string_hash(iter->m_key.c_str());
        else // m_type == INT
            key = int_hash(iter->m_key.c_str());

        // find appropriate bucket
        for (unsigned int i=0; i<servers.size(); ++i) {
            if (servers[i].start <= key && key <= servers[i].end) {
                server_index = i;
                break;
            }
        }

        // put request in bucket
        buckets[server_index].push_back(*iter);
    }
}

void print_buckets(vector< vector<Request> > &buckets) {
    for (unsigned int i=0; i<buckets.size(); ++i) {
        cout << "bucket #" << i+1 << endl;
        for (unsigned int j=0; j<buckets[i].size(); ++j) {
            cout << "\t" << buckets[i][j].to_string();
        }
        cout << endl;
    }
}

void print_servers(vector<server_info> &servers) {
    for (unsigned int i=0; i<servers.size(); ++i) {
        cout << server_info_to_string(servers[i]) << endl;
    }
}

