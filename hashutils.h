#ifndef HASHUTILS_H
#define HASHUTILS_H

#include <cstdlib>

const int TABLE_MAX_SIZE = 10240; // used strictly for the hash

//hash function for ints
//  NOTE: just converts string number to integer
int int_hash(const char* input)
{
    return atoi(input);
}

//hash function for strings supplied
//  NOTE: only used for string keys
int string_hash(const char* input)
{
    unsigned long hash = 5381;
    int c;

    while((c = *input++))
        hash = ((hash << 5) + hash) + c;

    return (int)(hash % TABLE_MAX_SIZE);
}

#endif

