#include "serversocket.h"
#include "clientsocket.h"
#include "parser.h"
#include "request.h"
#include "table.h"

#include "threadutils.h"
#include "socketutils.h"
#include "shmutils.h"
#include "hashutils.h"
#include "serverutils.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>

using namespace std;

// globals
vector<pthread_t> pool;
pthread_t slave;
char *c;
int shm_fd;

Table *it;
Table *st;

pthread_mutex_t read_mutex;
pthread_mutex_t write_mutex;
bool done = false;

// constants
const std::string OUT_PATH = "out.txt";

// functions
void register_server(server_info &si);

void* thread_work(void *data);
void dump_requests(ostream &os, string &buffer, const bool &is_done);

void* slave_work(void *data);
string process_requests(istream &is);
void slave_write(const string &path, const string &buffer);

int main (int argc, char **argv) {
    if (argc != 1) {
        cerr << "invalid number of arguments" << endl
             << "usage: [EXEC NAME]" << endl;
        return 1;
    }

    server_info si;
    register_server(si);

    // create & mmap shm
    int shm_fd;
    ostringstream oss;
    oss << SHM_PATH << "_" << si.port;
    init_shm(oss.str(), O_CREAT|O_EXCL|O_RDWR, S_IREAD|S_IWRITE, SHM_SIZE, shm_fd);
    mmap_shm(&c, SHM_SIZE, shm_fd);

    // init mutexes
    init_mutex(read_mutex);
    init_mutex(write_mutex);
    pthread_mutex_lock(&read_mutex); // start state should be locked

    // create pool of threads
    init_pool(pool, MAX_THREADS);

    // create & start slave thread
    pthread_create(&slave, NULL, slave_work, NULL);

    // create tables
    it = new Table(int_hash, TABLE_MAX_SIZE);
    st = new Table(string_hash, TABLE_MAX_SIZE);

    // create main server socket
    ServerSocket server(si.port);
    ServerSocket *new_sock;

    int i = 0;
    while (i < MAX_THREADS) {
        new_sock = new ServerSocket();

        // blocking call
        server.accept(*new_sock);

        if (pthread_create(&pool[i], NULL, thread_work, new_sock) != 0) {
            perror("pthread_create");
        }

        ++i;
    }

    // clean up
    server.close_socket();
    new_sock = NULL;

    pthread_join(slave, NULL);
    join_pool(pool);

    destroy_mutex(read_mutex);
    destroy_mutex(write_mutex);

    destroy_shm(c, oss.str(), SHM_SIZE, shm_fd);

    delete it;
    delete st;

    return 0;
}

void register_server(server_info &si) {
    // registers this server with the master server
    ClientSocket skt(LOCALHOST, REG_PORT);
    string buffer;

    skt.write(REG);
    skt.read(buffer);
    skt.write(CLO);

    string_to_server_info(buffer, si);
}

void* thread_work(void *data) {
    ServerSocket *ss = (ServerSocket*)data;
    string command, input, buffer;
    int backlog = 0;

    // wire oss to shm
    ostringstream oss;
    oss.rdbuf()->pubsetbuf(c, SHM_SIZE);

    // read initial RTS and send ACK
    ss->read(command);
    ss->write(ACK);

    // prime for loop
    ss->read(input);

    // fill buffer then dump into shm
    while (input != CLO) {
        buffer += input;

        ++backlog;

        if (backlog >= MAX_REQUESTS) {
            buffer += EOF;
            dump_requests(oss, buffer, false);

            backlog = 0;
            buffer = "";
        }

        ss->write(ACK);

        // read next input
        ss->read(input);
    }

    buffer += EOF;
    dump_requests(oss, buffer, true);

    ss->close_socket();
    delete ss;

    pthread_exit(NULL);
}

void dump_requests(ostream &os, string &buffer, const bool &is_done) {
    pthread_mutex_lock(&write_mutex);
    done = is_done;
    os << buffer;
    pthread_mutex_unlock(&read_mutex);
}

void* slave_work(void *data) {
    int i = MAX_THREADS;
    bool is_done = false;

    while (i--) {
        while (!is_done) {
            pthread_mutex_lock(&read_mutex);

            // read & process
            istringstream iss(c);
            string buffer = process_requests(iss);

            slave_write(OUT_PATH, buffer);

            is_done = done;
            if (done) done = false;

            pthread_mutex_unlock(&write_mutex);
        }

        is_done = false;
    }

    pthread_exit(NULL);
}

string process_requests(istream &is) {
    Table *tmp; // used to point to either table based on the request type

    ostringstream out_buffer;
    Request r;

    // handle each request until EOF
    while (is >> r.m_request && r.m_request != EOF) {
        is >> r.m_type >> r.m_key;

        // process each request
        if (r.m_type == INTEGER)
            tmp = it;
        else // iter->type == STRING
            tmp = st;

        // perform needed operation on tables
        if (r.m_request == PUT) {
            is >> r.m_number;
            getline(is, r.m_values);

            tmp->add(tmp->hash(r.m_key.c_str()), r.m_values);
        }
        else if (r.m_request == GET) {
            string res;
            if (!tmp->get(tmp->hash(r.m_key.c_str()), res))
                out_buffer << "a\n";
        }
        else if (r.m_request == DELETE) {
            tmp->remove(tmp->hash(r.m_key.c_str()));
        }
    }

    tmp = NULL;
    return out_buffer.str();
}

void slave_write(const string &path, const string &buffer) {
    // create out file
    int out_fd = open(path.c_str(), O_CREAT|O_WRONLY|O_APPEND, 0666);
    if (out_fd == -1)
        perror("out_file open/create");

    // write out file
    if (write(out_fd, buffer.c_str(), buffer.size()) == -1)
        perror("out_file write");

    close(out_fd);
}

