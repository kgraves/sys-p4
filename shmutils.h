#ifndef SHMUTILS_H
#define SHMUTILS_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

const int SHM_SIZE = 200000; // 200 kb
const std::string SHM_PATH = "/shm_table";

void init_shm(const std::string &path, const int &flags, const int &mode, const int &size, int &fd) {
    if ( (fd = shm_open(path.c_str(), flags, mode)) == -1 ) {
        perror("shm create");
    }

    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
    }
}

void mmap_shm(char **c, const int &size, int &fd) {
    *c = (char*)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
}

void destroy_shm(char *c, const std::string &path, const int &size, int &fd) {
    if (shm_unlink(path.c_str())) {
        perror("shm unlink");
    }

    munmap(c, size);
    c = NULL;
    close(fd);
}

#endif
