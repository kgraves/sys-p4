#include "table.h"
#include <assert.h>

Table::Table(int max_size) : size(max_size)
{
    this->hash_func_ptr = NULL;
    this->table = new std::string[max_size];

    for (int i=0; i<max_size; ++i)
        this->table[i] = "";
}

Table::Table(int (*hash_func_ptr)(const char* input),  int max_size) : size(max_size)
{
    this->hash_func_ptr = hash_func_ptr;
    this->table = new std::string[max_size];
}

int Table::hash(const char* input)
{
    if (this->hash_func_ptr)
        return this->hash_func_ptr(input);
    else
    {
        assert(false); //intentional
        return -1;
    }
}

bool Table::add(int index, std::string data)
{
    if (index > this->size)
        return false;
    else if (data == "")
        return false;

    this->table[index] = data;
    return true;
}

bool Table::get(int index, std::string &result)
{
    if (index > this->size || 
        this->table[index] == "")
    {
        result = "";
        return false;
    }

    assert(this->table[index] != "");
    result = this->table[index];
    return true;
}

bool Table::remove(int index)
{
    if (index > this->size || 
        this->table[index] == "")
        return false;

    assert(this->table[index] != "");
    this->table[index] = "";
    return true;
}

Table::~Table()
{
    //delete all pointers
    delete [] this->table;
}
