#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "request.h"

// constants
const int SMALL_BUFFER_SIZE = 512;
const int LARGE_BUFFER_SIZE = 32768;
const char LINE_DELIM = '\n';
const char PIECE_DELIM = '\t';
const char WORD_DELIM = ' ';

// enums
enum requests { GET, PUT, DELETE };

class Parser
{
    private:
        std::string m_in_path;
        int m_in_fd;

        std::vector<Request> m_requests;

        bool open_file(int &fd, const std::string &file_name, const int flags, const int modes=0);
        bool close_file(const int fd);

        size_t file_size(const int fd);

        bool parse(char** buffer, std::string &result, char delim, int &bytes_read);

        void read_file(int in_file, const int buffer_size);
        void mmap_file(char *buffer);

    public:
        Parser(const std::string &in_file);
        bool read_parse(const int chunk_size);
        bool mmap_parse();
        std::vector<Request>& requests() {return m_requests;};
};

#endif
