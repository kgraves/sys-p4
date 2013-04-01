#ifndef THREADUTILS_H
#define THREADUTILS_H

#include <vector>
#include <pthread.h>
#include <cstdio>

// constants
const int MAX_THREADS = 10;

// typedefs
//typedef std::vector<pthread_t*>::iterator ptiter;
typedef std::vector<pthread_t>::iterator ptiter;

// function that initializes all threads in given pool
void init_pool(std::vector<pthread_t> &pool, const int &size) {
    for (int i=0; i<size; ++i) {
        pthread_t pt;
        pool.push_back(pt);
    }
}

void join_pool(std::vector<pthread_t> &pool) {
    void **ret = NULL;

    for (ptiter iter=pool.begin(); iter != pool.end(); ++iter) {
        if (pthread_join(*iter, ret) == -1)
            perror("join error");
    }
}


void init_mutex(pthread_mutex_t &mutex) {
    pthread_mutex_init(&mutex, NULL);
}

void destroy_mutex(pthread_mutex_t &mutex) {
    pthread_mutex_destroy(&mutex);
}

/*
// function that initializes all threads in given pool
void init_pool(std::vector<pthread_t*> &pool, const int &size) {
    for (int i=0; i<size; ++i) {
        pool.push_back(new pthread_t());
    }
}

void join_pool(std::vector<pthread_t*> &pool) {
    void **ret = NULL;

    for (ptiter iter=pool.begin(); iter != pool.end(); ++iter) {
        if (pthread_join(*(*iter), ret) == -1)
            perror("join error");

        delete *iter;
    }
}

// function that deallocates the pool
void remove_pool(std::vector<pthread_t*> &pool, const int &size) {
    for (int i=0; i<size; ++i) {
        if (pool[i]) {
            delete pool[i];
            //printf("\tdeleted thread pointer\n");
        }
    }
}
*/

#endif

