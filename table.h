#ifndef TABLE_H
#define TABLE_H

#include <string>

class Table
{
    public:
        Table(int max_size);
        Table(int (*hash_func_ptr)(const char* input), int max_size);
        ~Table();

        int hash(const char* input);

        bool add(int index, std::string data);
        bool get(int index, std::string &result);
        bool remove(int index);

    private:
        const int size;
        std::string *table;

        //function pointer to specified hash function
        int (*hash_func_ptr)(const char* input);
};

#endif
