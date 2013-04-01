#include "table.h"
#include "parser.h"
#include "request.h"
#include "logger.h"
#include "socket.h"
#include "hashutils.h"
#include "threadutils.h"
#include "shmutils.h"
#include "socketutils.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

using namespace std;

// Globals
Logger *log = Logger::instance();

vector<pthread_t*> pool;
int work_mutex;
pthread_mutex_t read_mutex;

char *c;
int default_port = 4243;

// Constants
const std::string OUT_PATH = "out.txt";

// Functions
void* child_init(void *args);
vector<Request> child_work(const string &in_path);
void child_write(ostream &os, vector<Request> &requests);
string parent_work(istream &is, int &work_mutex, pthread_mutex_t &read_mutex, int num_processes);
void parent_write(const string &path, const string &buffer);
void create_shm(const string &path, const int &flags, const int &mode, int &shm_fd);
void remove_shm(const string &path);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "usage: [exec name] [host] [port]" << endl;
        perror("Illegal number of arguments");
    }
    else
    {
        int index=1; //, num_processes = argc-1;
        int shm_fd;
        int main_fd;

        // create shm & mmap
        init_shm(SHM_PATH, O_CREAT|O_EXCL|O_RDWR, S_IREAD|S_IWRITE, SHM_SIZE, shm_fd);
        mmap_shm(&c, SHM_SIZE, shm_fd);

        // create mutex
        pthread_mutex_init(&read_mutex, NULL);
        work_mutex = 1;

        Socket skt(argv[1], argv[2]);
        skt.create_server_socket();

        // creates all threads in pool
        init_pool(pool, MAX_THREADS);

        // block until we get a connection
        while (index < BACKLOG && (main_fd = skt.create_server_accept())) {
            // create package
            thread_package *tp = new thread_package();
            tp->index = index;
            tp->fd = main_fd;

            // spawn thread
            pthread_create(pool[index], NULL, child_init, tp);

            ++index;
        }

        // parent
        //istringstream iss(c);
        //string buffer = parent_work(iss, work_mutex, read_mutex, num_processes);
        //parent_write(OUT_PATH, buffer);

        // clean up
        join_pool(pool);
        destroy_shm(c, SHM_PATH, SHM_SIZE, shm_fd);
        destroy_mutex(read_mutex);
    }

    log = NULL;
    return 0;
}

void* child_init(void *args) {

    thread_package *pack = (thread_package*)args;

    //socket_package sp;
    //read_data(pack->fd, &sp, sizeof(sp));
    //int groups;

    //if (sp.command == RTS) {
    //groups = sp.data;
    //acknowledge(pack->fd);

    while (work_mutex < pack->index) { 
        sched_yield(); 
    }

    ostringstream oss;
    oss.rdbuf()->pubsetbuf(c, SHM_SIZE);

    //vector<Request> requests = child_work(pack->input_file);
    //child_write(oss, requests);

    if (pthread_mutex_unlock(&read_mutex) == -1)
        perror("read_mutex release");
    //}
    //else
        //close_connection(pack->fd);


    delete pack;
    pack = NULL;

    pthread_exit(NULL);
}

vector<Request> child_work(const string &in_path)
{
    Parser p1(in_path);
    p1.mmap_parse();
    return p1.requests();
}

void child_write(ostream &os, vector<Request> &requests)
{
    //log->log("child_write begin");
    // write each request
    for (vector<Request>::iterator iter=requests.begin(); iter!=requests.end(); ++iter) {
        os << iter->request() << SEPARATOR <<
                iter->type() << SEPARATOR <<
                iter->key() << SEPARATOR;

        if (iter->request() == PUT) {
            os << iter->number() << SEPARATOR <<
                    iter->values() << endl;
        }
    }
    //log->log("child_write mid");

    // used to let parent know done with transmission
    os << EOF << SEPARATOR;
    //log->log("child_write end");
}

string parent_work(istream &is, int &work_mutex, pthread_mutex_t &read_mutex, int num_processes) {
    Table *it = new Table(int_hash, TABLE_MAX_SIZE);
    Table *st = new Table(string_hash, TABLE_MAX_SIZE);
    Table *tmp; // used to point to either table based on the request type

    ostringstream out_buffer;
    Request r;

    while (num_processes--) {
        pthread_mutex_lock(&read_mutex);

        while (is >> r.m_request && r.m_request != EOF) {
            // read data
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

        ++work_mutex;

        is.seekg(ios_base::beg);
    }

    // clean up
    delete it;
    delete st;
    tmp = NULL;

    return out_buffer.str();
}

/*
string parent_work(istream &is, sem_t *sem_work, sem_t *sem_read, int num_processes)
{
    Table *it = new Table(int_hash, TABLE_MAX_SIZE);
    Table *st = new Table(string_hash, TABLE_MAX_SIZE);
    Table *tmp; // used to point to either table based on the request type

    ostringstream out_buffer;
    Request r;

    while (num_processes--) {
        if (sem_wait(sem_read))
            perror("sem_wait error");

        while (is >> r.m_request && r.m_request != EOF) {
            // read data
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

        if (sem_post(sem_work))
            perror("sem_wait error");

        is.seekg(ios_base::beg);
    }

    // clean up
    delete it;
    delete st;
    tmp = NULL;

    return out_buffer.str();
}
*/

void parent_write(const string &path, const string &buffer)
{
    // create out file
    int out_fd = open(path.c_str(), O_CREAT|O_WRONLY|O_APPEND, 0666);
    if (out_fd == -1)
        perror("out_file open/create");

    // write out file
    if (write(out_fd, buffer.c_str(), buffer.size()) == -1)
        perror("out_file write");

    close(out_fd);
}

void create_shm(const string &path, const int &flags, const int &mode, int &shm_fd)
{
    if ( (shm_fd = shm_open(path.c_str(), flags, mode)) == -1 ) {
        perror("shm create");
    }
}

void remove_shm(const string &path)
{
    if (shm_unlink(path.c_str())) {
        perror("shm unlink");
    }
}

